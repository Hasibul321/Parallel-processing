#include<iostream>
#include<mpi.h>

using namespace std;
int main(int argc,char **argv)
{
    MPI_Init(&argc, &argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    double start_time,end_time;

    int num;
    int row;
    int col;
    int B_col;

    if(rank == 0)
    {
        printf("Enter the number of matrices : ");
        scanf("%d",&num);
        printf("\nEnter the number of row of A matrix : ");
        scanf("%d",&row);
        printf("\nEnter the number of col of A matrix : ");
        scanf("%d",&col);
        printf("\nEnter the number of column of B matrix : ");
        scanf("%d",&B_col);
    }
    MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&row,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&col,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&B_col,1,MPI_INT,0,MPI_COMM_WORLD);

    int root = 0;
    int matrixA[num][row][col];
    int matrixB[num][col][B_col];
    srand(time(0));

    if(rank == root)
    {
        for(int i =0;i<num;i++)
        {
            for(int j = 0;j<row;j++)
            {
                for(int k =0;k<col;k++)
                {
                    matrixA[i][j][k] = rand() % 10;
                }
            }

            for(int r =0;r<col;r++)
            {
                for(int col =0;col<B_col;col++)
                {
                    matrixB[i][r][col] = rand() % 10;
                }
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    int localMatrixA [num/size][row][col];
    int localMatrixB [num/size][col][B_col];

    int resultMatrix [num/size][row][B_col];

    MPI_Scatter(matrixA,(num/size)*row*col,MPI_INT,localMatrixA,(num/size)*row*col,MPI_INT,root,MPI_COMM_WORLD);
    MPI_Scatter(matrixB,(num/size)*row*col,MPI_INT,localMatrixB,(num/size)*row*col,MPI_INT,root,MPI_COMM_WORLD);
    
    //matrix multiplication
    for(int i =0;i<num/size;i++)
    {
        for(int j = 0;j<row;j++)
        {
            for(int k =0;k<B_col;k++)
            {
                resultMatrix[i][j][k] = 0;
                for(int l =0;l<col;l++)
                {
                    resultMatrix[i][j][k] += localMatrixA[i][j][l] * localMatrixB[i][l][k];
                }
            }
        }
    }
    end_time = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    printf("process %d takes %f seconds \n",rank,end_time-start_time);

    int gatherMat[num][row][B_col];
    MPI_Gather(resultMatrix,(num / size) * row * B_col,MPI_INT, gatherMat,(num / size) * row * B_col,MPI_INT,root,MPI_COMM_WORLD);

    if(rank == root)
    {
        cout<<"All" <<num <<"Matrix are :";
        cout<<endl;
        for(int i =0;i<num;i++)
        {
            cout<<"Matrix "<<i <<":"<<endl;
            for(int j =0;j<row;j++)
            {
                for(int k =0;k<B_col;k++)
                {
                    cout<<gatherMat[i][j][k]<<" ";
                }
                cout<<endl;
            }
        }

    }

    MPI_Finalize();
    return 0;
}