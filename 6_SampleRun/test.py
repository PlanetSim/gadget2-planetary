import numpy as npy
import math

class EOStable:
    
    def __init__(self):
        self.ND=0
        self.NS=0
        self.rho=npy.zeros(self.ND)
        self.S=npy.zeros(self.NS)
        self.P=npy.zeros(self.ND*self.NS)
        self.T=npy.zeros(self.ND*self.NS)
        self.U=npy.zeros(self.ND*self.NS)
        self.cs=npy.zeros(self.ND*self.NS)
        
    def load(self,fname):
        data=([])
        with open(fname,'r') as file:
            for line in file:
                data=npy.append(data,line.strip('\n').split(' '))
        data=data[data!=''].astype(float)
        self.ND=data[0].astype(int)
        self.NS=data[1].astype(int)
        self.rho=data[2:2+self.ND]
        self.S=data[2+self.ND:2+self.ND+self.NS]
        self.P=data[2+self.ND+self.NS:2+self.ND+self.NS+self.ND*self.NS].reshape(self.NS,self.ND)
        self.T=data[2+self.ND+self.NS+self.ND*self.NS:2+self.ND+self.NS+2*self.ND*self.NS].reshape(self.NS,self.ND)
        self.U=data[2+self.ND+self.NS+2*self.ND*self.NS:2+self.ND+self.NS+3*self.ND*self.NS].reshape(self.NS,self.ND)
        self.cs=data[2+self.ND+self.NS+3*self.ND*self.NS:2+self.ND+self.NS+4*self.ND*self.NS].reshape(self.NS,self.ND)
    


    def view(self,q='T',Slow=None,Shigh=None,rholow=None,rhohigh=None):
        if Slow is None:
            Slow=self.S.min()
        if Shigh is None:
            Shigh=self.S.max()
        if rholow is None:
            rholow=self.rho.min()
        if rhohigh is None:
            rhohigh=self.rho.max()
        if q=='T':
            print '-> rho:', self.rho[npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
            print 'v  S:', self.S[npy.logical_and(self.S>=Slow,self.S<=Shigh)]
            print 'T:', (self.T[npy.logical_and(self.S>=Slow,self.S<=Shigh)])[:,npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
        if q=='P':
            print '-> rho:', self.rho[npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
            print 'v  S:', self.S[npy.logical_and(self.S>=Slow,self.S<=Shigh)]
            print 'P:', (self.P[npy.logical_and(self.S>=Slow,self.S<=Shigh)])[:,npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
        if q=='U':
            print '-> rho:', self.rho[npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
            print 'v  S:', self.S[npy.logical_and(self.S>=Slow,self.S<=Shigh)]
            print 'U:', (self.U[npy.logical_and(self.S>=Slow,self.S<=Shigh)])[:,npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
        if q=='cs':
            print '-> rho:', self.rho[npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
            print 'v  S:', self.S[npy.logical_and(self.S>=Slow,self.S<=Shigh)]
            print 'cs:', (self.cs[npy.logical_and(self.S>=Slow,self.S<=Shigh)])[:,npy.logical_and(self.rho>=rholow,self.rho<=rhohigh)]
        




fo=EOStable()

fo.load('material02.txt')
fo.view(q='U')
fo.view(Slow=1.9e7,Shigh=2.9e7)
fo.view(Slow=1.9e7,Shigh=2.5e7,rholow=3.2,rhohigh=3.5)
fo.view(q='P',Slow=2.7e7,Shigh=4e7,rholow=3.0,rhohigh=3.35)
