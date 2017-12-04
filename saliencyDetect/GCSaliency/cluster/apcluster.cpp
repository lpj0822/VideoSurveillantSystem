#include "apcluster.h"

APCluster::APCluster(int N,int prefType, double lambda, int maximum_iterations, int converge_iterations)
{
    init();
    this->N=N;
    this->prefType=prefType;
    this->lambda=lambda;
    this->maximum_iterations=maximum_iterations;
    this->converge_iterations=converge_iterations;
}

APCluster::~APCluster()
{

}


/*Build graph from sparse similarity matrix stored in COO format.
 * Input specification is following:
 * First line contains an integer standing for the size of the matrix.
 * Following lines each contain two integers and a real number standing for
 * the row index i, the column index j and the similarity s(i,j) respectively.
 * Input ends with an end-of-file.
 * Note that this function does not check any errors in the given input.
 * Parameter:
 * input: Input file handle.
 * prefType:
 *   1: use median of similarities as preference
     2: use minimum of similarities as preference
     3: use min - (max - min) of similarities as preference
*/
Graph* APCluster::buildGraph(cv::Mat src)
{
    Graph* graph = new Graph;
    graph->n=N;
    graph->outEdges = new Edges[N];
    graph->inEdges = new Edges[N];
    std::vector<Edge>& edges = graph->edges;

    // read similarity matrix
    for(int i=0;i<src.rows;i++)
    {
        const double* data=src.ptr<double>(i);
        for(int j=0;j<src.cols;j++)
        {
            if (i == j)
            {
                continue;
            }
            edges.push_back(Edge(i, j, data[j]));
        }
    }
    //calculate preferences
    double pref;
    if (prefType == 1)
    {
        std::sort(edges.begin(), edges.end());
        int m = (int)edges.size();
        pref = (m % 2) ? edges[m/2].s : (edges[m/2 - 1].s + edges[m/2].s) / 2.0;
    }
    else if (prefType == 2)
    {
        pref = std::min_element(edges.begin(), edges.end())->s;
    }
    else if (prefType == 3)
    {
        double minValue = std::min_element(edges.begin(), edges.end())->s;
        double maxValue = std::max_element(edges.begin(), edges.end())->s;
        pref = 2*minValue - maxValue;
    }
    else
    {
        assert(false);      // invalid prefType
    }
    for (int i = 0; i < graph->n; ++i)
    {
        edges.push_back(Edge(i, i, pref));
    }

    /*
    for(int i=0;i<src.rows;i++)
    {
        const double* data=src.ptr<double>(i);
        for(int j=0;j<src.cols;j++)
        {
            edges.push_back(Edge(i, j, data[j]));
        }
    }
    */

    for (size_t i = 0; i < edges.size(); ++i)
    {
        Edge* p = &edges[i];
        // add small noise to avoid degeneracies
        p->s += (1e-16 * p->s + 1e-300) * (rand() / (RAND_MAX + 1.0));
        // add out/in edges to vertices
        graph->outEdges[p->src].push_back(p);
        graph->inEdges[p->dst].push_back(p);
    }

    return graph;
}

void APCluster::destroyGraph(Graph* graph)
{
    if(graph->outEdges)
    {
        delete [] graph->outEdges;
        graph->outEdges=NULL;
    }
    if(graph->inEdges)
    {
        delete [] graph->inEdges;
        graph->inEdges=NULL;
    }
    if(graph)
    {
        delete graph;
        graph=NULL;
    }
}

void APCluster::updateResponsibilities(Graph* graph)
{
    for (int i = 0; i < graph->n; ++i)
    {
        Edges& edges = graph->outEdges[i];
        int m =(int)edges.size();
        double max1 = -HUGE_VAL, max2 = -HUGE_VAL;
        double argmax1 = -1;
        for (int k = 0; k < m; ++k)
        {
            double value = edges[k]->s + edges[k]->a;
            if (value > max1)
            {
                std::swap(max1, value);
                argmax1 = k;
            }
            if (value > max2)
            {
                max2 = value;
            }
        }
        // update responsibilities
        for (int k = 0; k < m; ++k)
        {
            if (k != argmax1)
            {
                update(edges[k]->r, edges[k]->s - max1);
            }
            else
            {
                update(edges[k]->r, edges[k]->s - max2);
            }
        }
    }
}

void APCluster::updateAvailabilities(Graph* graph)
{
    for (int k = 0; k < graph->n; ++k)
    {
        Edges& edges = graph->inEdges[k];
        int m = (int)edges.size();
        // calculate sum of positive responsibilities
        double sum = 0.0;
        for (int i = 0; i < m-1; ++i)
        {
            sum += std::max(0.0, edges[i]->r);
        }
        // calculate availabilities
        double rkk = edges[m-1]->r;
        for (int i = 0; i < m-1; ++i)
        {
            update(edges[i]->a, std::min(0.0, rkk + sum - std::max(0.0, edges[i]->r)));
        }
        // calculate self-availability
        update(edges[m-1]->a, sum);
    }
}

bool APCluster::updateExamplars(Graph* graph, std::vector<int>& examplar)
{
    bool changed = false;
    for (int i = 0; i < graph->n; ++i)
    {
        Edges& edges = graph->outEdges[i];
        int m = (int)edges.size();
        double maxValue = -HUGE_VAL;
        int argmax = i;
        for (int k = 0; k < m; ++k)
        {
            double value = edges[k]->a + edges[k]->r;
            if (value > maxValue)
            {
                maxValue = value;
                argmax = edges[k]->dst;
            }
        }
        if (examplar[i] != argmax)
        {
            examplar[i] = argmax;
            changed = true;
        }
    }
    return changed;
}

/*Cluster data points with Affinity Propagation.
 * Parameters:
     input: Input file which contains sparse similarity matrix. see buildGraph().
     prefType: Specify what kind of preference we use. see buildGraph().
     lambda: The lambda factor. (0.5 <= lambda < 1.0)
     maximum_iterations: The maximum number of iterations.
     converge_iterations: Specify how many iterations this algorithm stops when examplars
            did not change for.
 *Returns:
     Array of examplars of corresponding data points.
*/
std::vector<int> APCluster::affinityPropagation(cv::Mat src)
{
    assert(0.499 < lambda && lambda < 1.0);
    Graph* graph = buildGraph(src);
    std::vector<int> examplar(graph->n, -1);

    for (int i = 0, nochange = 0; i < maximum_iterations && nochange < converge_iterations; ++i, ++nochange)
    {
        updateResponsibilities(graph);
        updateAvailabilities(graph);
        if (updateExamplars(graph, examplar))
        {
            nochange = 0;
        }
    }

    destroyGraph(graph);
    return examplar;

    /*
    std::vector<int> examplar(N,0);
    double **S=NULL;
    double **R=NULL;
    double **A=NULL;
    S=new double*[N];
    R=new double*[N];
    A=new double*[N];
    for(int loop=0;loop<N;loop++)
    {
        S[loop]=new double[N];
        R[loop]=new double[N];
        A[loop]=new double[N];
    }
    for(int i=0;i<src.rows;i++)
    {
        const double* data=src.ptr<double>(i);
        for(int j=0;j<src.cols;j++)
        {
            S[i][j]=data[j];
            R[i][j]=0;
            A[i][j]=0;
        }
    }

    for(int m=0; m<maximum_iterations; m++)
    {
        //update responsibility
        for(int i=0; i<N; i++)
        {
            for(int k=0; k<N; k++)
            {
                double max = -1e100;
                for(int kk=0; kk<k; kk++)
                {
                    if(S[i][kk]+A[i][kk]>max)
                        max = S[i][kk]+A[i][kk];
                }
                for(int kk=k+1; kk<N; kk++)
                {
                    if(S[i][kk]+A[i][kk]>max)
                        max = S[i][kk]+A[i][kk];
                }
                R[i][k] = (1-lambda)*(S[i][k] - max) + lambda*R[i][k];
            }
        }
        //update availability
        for(int i=0; i<N; i++)
        {
            for(int k=0; k<N; k++)
            {
                if(i==k)
                {
                    double sum = 0.0;
                    for(int ii=0; ii<i; ii++)
                    {
                        sum += std::max(0.0, R[ii][k]);
                    }
                    for(int ii=i+1; ii<N; ii++)
                    {
                        sum += std::max(0.0, R[ii][k]);
                    }
                    A[i][k] = (1-lambda)*sum + lambda*A[i][k];
                }
                else
                {
                    double sum = 0.0;
                    int maxik = std::max(i, k);
                    int minik = std::min(i, k);
                    for(int ii=0; ii<minik; ii++)
                    {
                        sum += std::max(0.0, R[ii][k]);
                    }
                    for(int ii=minik+1; ii<maxik; ii++)
                    {
                        sum += std::max(0.0, R[ii][k]);
                    }
                    for(int ii=maxik+1; ii<N; ii++)
                    {
                        sum += std::max(0.0, R[ii][k]);
                    }
                    A[i][k] = (1-lambda)*std::min(0.0, R[k][k]+sum) + lambda*A[i][k];
                }
            }
        }
    }

    //find the exemplar
    std::vector<int> center;
    for(int i=0; i<N; i++)
    {
        if(R[i][i] + A[i][i]>0)
        {
            center.push_back(i);
        }
    }
    //data point assignment, idx[i] is the exemplar for data point i
    for(int i=0; i<N; i++)
    {
        int idxForI = 0;
        double maxSim = -1e100;
        for(int j=0; j<center.size(); j++)
        {
            int c = center[j];
            if (S[i][c]>maxSim)
            {
                maxSim = S[i][c];
                idxForI = c;
            }
        }
        examplar.push_back(idxForI);
    }

    for(int loop=0;loop<N;loop++)
    {
        if(S[loop])
        {
            delete[] S[loop];
            S[loop]=NULL;
        }
        if(R[loop])
        {
            delete[] R[loop];
            R[loop]=NULL;
        }
        if(A[loop])
        {
            delete[] A[loop];
            A[loop]=NULL;
        }
    }
    if(S)
    {
        delete[] S;
        S=NULL;
    }
    if(R)
    {
        delete[] R;
        R=NULL;
    }
    if(A)
    {
        delete[] A;
        A=NULL;
    }
    return examplar;
    */
}

int APCluster::reMapIdx(std::vector<int> &mapIdx)
{
    int N = (int)mapIdx.size();
    int newCount = 0;
    std::map<int, int> idxCount, oldNewIdx;
    idxCount.clear();
    for (int i = 0; i < N; i++)
    {
        if (idxCount.find(mapIdx[i]) == idxCount.end())
            oldNewIdx[mapIdx[i]] = newCount++, idxCount[mapIdx[i]]++;
        mapIdx[i] = oldNewIdx[mapIdx[i]];
    }
    return (int)idxCount.size();
}


void APCluster::init()
{

}
