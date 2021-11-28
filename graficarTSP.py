    # -*- coding: utf-8 -*-
"""
Universidad de Pamplona
Facultad de Ingenierías y Arquitectura
Programa de Ingeniería de Sistemas
Profesor: José Orlando Maldonado Bautista

ALGORITMO GENETICO SIMPLE APLICADO AL TSP
"""
import numpy as np
import matplotlib.pyplot as plt

def graficarCamino(coordenadas,camino):
    plt.grid()
    
    [nCiudades,nc] = coordenadas.shape
    
    c = coordenadas
    r = camino
    plt.scatter(c[:,0],c[:,1])

    for i in range(nCiudades):
        if (i<nCiudades-1):
            j = i+1
        else:
            j=0
        p1 = np.array([c[r[i],0],c[r[j],0]])
        p2 = np.array([c[r[i],1],c[r[j],1]]) 
        plt.plot(p1,p2)
    
    plt.show()
        
######################## CODIGO PRINCIPAL ########################
fichero01 = 'coordenadasX.txt'
raw_data = open(fichero01)
cX = np.loadtxt(raw_data, delimiter=",")

fichero02 = 'coordenadasY.txt'
raw_data = open(fichero02)
cY = np.loadtxt(raw_data, delimiter=",")
print(cX.size, 'size x')
print(cY.size, 'size y')
print(cX, 'x') 
print(cY, 'y')
coordenadas = np.concatenate([cX.reshape(cX.size,1),cY.reshape(cY.size,1)],axis = 1)
coordenadas = coordenadas.astype(int)

fichero01 = 'caminoInicial.txt'
raw_data = open(fichero01)
camino = np.loadtxt(raw_data, delimiter=",").astype(int) 
camino = camino - 1

graficarCamino(coordenadas,camino)

fichero01 = 'caminoFinalTour _python.txt'
raw_data = open(fichero01)
camino = np.loadtxt(raw_data, delimiter=",").astype(int) 
camino = camino - 1

graficarCamino(coordenadas,camino)

fichero01 = 'caminoFinal.txt'
raw_data = open(fichero01)
camino = np.loadtxt(raw_data, delimiter=",").astype(int) 
camino = camino - 1

graficarCamino(coordenadas,camino)

