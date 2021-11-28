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
        
        
def leerFicheroParsearValores(nombreArchivo: str):
    raw_data = open(nombreArchivo)
    valores = np.loadtxt(raw_data, delimiter=",")
    return valores

######################## CODIGO PRINCIPAL ########################

if __name__ == '__main__':
    cX = leerFicheroParsearValores('coordenadasX.txt')
    cY = leerFicheroParsearValores('coordenadasY.txt')
    caminoInicial = leerFicheroParsearValores('caminoInicial.txt').astype(int)
    caminoFinal = leerFicheroParsearValores('caminoFinal.txt').astype(int)
    # print(cX) 
    # print(cY)
    coordenadas = np.concatenate([cX.reshape(cX.size,1),cY.reshape(cY.size,1)],axis = 1)
    coordenadas = coordenadas.astype(int)
    caminoInicial = caminoInicial - 1
    caminoFinal = caminoFinal - 1
    graficarCamino(coordenadas,caminoInicial)
    graficarCamino(coordenadas,caminoFinal)
