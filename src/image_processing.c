#include <mpi.h>
#include <stdio.h>

int N;
int disp_part;
int taskid;
int numtasks;
int *inp_matrix;
int disp_arr[32];
int sendcount_arr[32];
int recvcount_arr[32];


int main(int argc, char *argv[]){
    
    N = atoi(argv[1]);

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

    if (numtasks < 2 ) {
      printf("Need at least two MPI tasks. Quitting...\n");
      MPI_Abort(MPI_COMM_WORLD, rc);
      exit(1);
    }
    disp_idx_jmp = (N * N)/(numtasks - 1);
    send_per_worker = disp_idx_jmp + (2 * N);

    init_matrix();
    scatter_gather();

    MPI_Finalize();
    return 0;
}

void scatter_gather(){

    int *imm_buf;
    if (taskid != 0) {
        imm_buf = malloc(send_per_worker * send_per_worker * sizeof(int));
    }
    
    MPI_Scatterv(inp_matrix, sendcount_arr, disp_arr, MPI_INT, imm_buf, send_per_worker,,MPI_INT, 0, MPI_COMM_WORLD);    

    int *calc_reorder_buf[62500 * sizeof(int)];
    if (taskid != 0) {
       int *sub_mat = (int *)imm_buf; 

       for (int j = 0; j < N - 3; j++) {

           sub_calc = sub_mat[0][j + 1] + sub_mat[0][j + 2];
           sub_left = sub_mat[0][j] + sub_calc;
           sub_right = sub_calc + sub_mat[0][j + 3]; 
           
           calc_reorder_buf[0][j] += sub_left;

           calc_reorder_buf[0][j + 1] += sub_right;

           sub_calc = sub_mat[1][j + 1] + sub_mat[1][j + 2];
           sub_left = sub_mat[1][j] + sub_calc;
           sub_right = sub_calc + sub_mat[1][j + 3]; 
           
           calc_reorder_buf[0][j] += sub_left;
           calc_reorder_buf[1][j] += sub_left;

           calc_reorder_buf[0][j + 1] += sub_right;
           calc_reorder_buf[1][j + 1] += sub_right;
       }

       for (int i = 2; i < N - 3 ; i++) {

           for (int j = 0; j < N - 3; j+=2) {

               sub_calc = sub_mat[i][j + 1] + sub_mat[i][j + 2];
               sub_left = sub_mat[i][j] + sub_calc;
               sub_right = sub_calc + sub_mat[i][j + 3]; 
               
               calc_reorder_buf[i - 2][j] += sub_left;
               calc_reorder_buf[i - 1][j] += sub_left;
               calc_reorder_buf[i][j] = sub_left;

               calc_reorder_buf[i - 2][j + 1] += sub_right;
               calc_reorder_buf[i - 1][j + 1] += sub_right;
               calc_reorder_buf[i][j + 1] += sub_right;
           }
       }

       for (int j = 0; j < N - 3; j++) {

           sub_calc = sub_mat[N - 1][j + 1] + sub_mat[N - 1][j + 2];
           sub_left = sub_mat[N - 1][j] + sub_calc;
           sub_right = sub_calc + sub_mat[N - 1][j + 3]; 
           
           calc_reorder_buf[N - 4][j] += sub_left;

           calc_reorder_buf[N - 4][j + 1] += sub_right;

           sub_calc = sub_mat[N - 2][j + 1] + sub_mat[1][j + 2];
           sub_left = sub_mat[N - 2][j] + sub_calc;
           sub_right = sub_calc + sub_mat[N - 2][j + 3]; 
           
           calc_reorder_buf[N - 3][j] += sub_left;
           calc_reorder_buf[N - 4][j] += sub_left;

           calc_reorder_buf[N - 4][j + 1] += sub_right;
           calc_reorder_buf[N -3][j + 1] += sub_right;
       }

       for (int i = 1; i < n; i++) {

           for (int j = 1; j < n; i++) {

               calc_reorder_buf[i][j] += sub_mat[i][j];
               calc_reorder_buf[i][j] /= 10;
           }
       }
   }

   MPI_Gatherv(calc_reorder_buf, send_per_worker, MPI_INT, matrix, recvcount_arr, disp_arr, 0, MPI_COMM_WORLD);

   if (taskid == 0) {
       printf("Master matrix is :\n\n")
       for (int i = 0; i < N - 1; i++) {
           for (int j = 0; j < N - 1; j++) {

               printf("%d, ", matrix[i][j]);

           }
           printf("\n");
       }
   }
}

void init_matrix(){
    srand(1);
    matrix = malloc(N * N * sizeof(int));

    printf("--------Initialzing matrix\n\n");
    for (int i = 0; i < N; i++){
        disp_arr[i] = (i) * disp_idx_jmp;
        sendcount_arr[i] = send_per_worker;
        recvcount_arr[i] = disp_idx_jmp;
        for (int j = 0; j < N; j++){
            matrix[i][j] = (rand()/RAND_MAX) * 255;
            printf("%d, "matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n-------Finished init\n");
}



     
