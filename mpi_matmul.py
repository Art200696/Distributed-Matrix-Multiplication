from mpi4py import MPI
import numpy as np

def init_same():
    for i in range(N):
        for j in range(M):
            a[i][j] = b[i][j] = i * j * j + i + j + 1

N, M, master_id = 21, 21, 0
max_value = 100

a = np.random.randint(max_value, size=(N, M))
b = np.random.randint(max_value, size=(N, M))

if N != M:
    raise ValueError("The dimensions do not corresponds to square matrices")

c = np.zeros(shape=(N, M))

comm = MPI.COMM_WORLD
core_id = MPI.Get_processor_name()
cores = comm.Get_size()
rank = comm.Get_rank()

print ("Process %d started.\n" % rank)
print ("Running from processor {}, rank {} out of {} processors.\n".format(core_id, rank, cores))

portion = N if cores < 2 else N // (cores - 1)

if portion < 1:
    raise ValueError("The number of splits is not positive")

comm.Barrier()
init_same()

if rank == master_id: # master node
    print ("Multiplication Starts")    
    for i in range(1, cores):
        start_pos = (i-1) * portion
        row = a[start_pos, :]
        comm.send(start_pos, dest=i, tag=i)
        comm.send(row, dest=i, tag=i)
        for j in range(portion):
            comm.send(a[j+start_pos,:], dest=i, tag=j+start_pos)

comm.Barrier()

if rank != master_id: # slave nodes
    offset = comm.recv(source=master_id, tag=rank)
    received_data = comm.recv(source=master_id, tag=rank)
    for j in range(int(portion) - 1):
        c = comm.recv(source=master_id, tag=j+offset + 1)
        received_data = np.vstack((received_data, c))
        
    t_start = MPI.Wtime()
    for i in range(int(portion)):
        res = np.zeros(shape=(M))
        if (portion == 1):
            r = received_data
        else:
            r = received_data[i,:]
        ptr = 0
        for j in range(M):
            col = b[:,j]
            for x in range(M):
                res[j] = res[j] + (r[x]*col[x])
            ptr = ptr + 1
        if i > 0:
            send = np.vstack((send, res))
        else:
            send = res
    
    print("Elapsed time for the process %d is %.4fs.\n" %(rank, MPI.Wtime() - t_start))
    comm.Send([send, MPI.FLOAT], dest=master_id, tag=rank)

comm.Barrier()

if rank == master_id:  
    res1 = np.zeros(shape=(portion, M))
    comm.Recv([res1, MPI.FLOAT], source=1, tag=1)
    kl = np.vstack((res1))
    for i in range(2, cores):
        col= np.zeros(shape=(portion, M))
        comm.Recv([col, MPI.FLOAT], source=i, tag=i)
        kl = np.vstack((kl, col.astype(int)))
    print ("Process ended")
    print ("Final result is:")
    print(kl)
    print("a @ b =", a @ b)
    #print("a =", a)
    #print("b =", b)
    print(np.array_equal(a @ b, kl))
    
comm.Barrier()