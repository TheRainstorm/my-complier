import numpy as np 

A = np.array([[10,-12,-6],[5,-5,-4],[-1,0,3]])
x1 = np.array([[1],[0],[0]])
x2 = np.array([[0],[1],[0]])
x3 = np.array([[0],[0],[1]])
X = [x1,x2,x3]

for i in range(0,4):
    for j in range(0,3):
        for k in range(0,100):
            try:
                u = X[j]/np.linalg.norm(X[j],2)
                lam = np.dot(u.T,np.dot(A,u))
                temp = np.linalg.inv(A-lam*np.eye(3,3))
                X[j] = np.dot(temp,u)
            except:
                break
        u = X[j]/np.linalg.norm(X[j],2)
        lam = np.dot(u.T,np.dot(A,u))
        print('特征向量',u)
        print('特征值',lam)
