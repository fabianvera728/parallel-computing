import sys
from subprocess import check_output
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

with open(f'{sys.argv[1]}.txt') as f:
    lines = f.readlines()

data = []
for i in lines:
    cad = i.strip()
    if cad != "":
        data.append(cad)

parallel_time = []
speedups = []
efficiencies = []
databyiterations = "".join(data).split("**__**")
threads = len(databyiterations)

databyiterations.__delitem__(0)
item = 1
for i in databyiterations:
    dataofiteration = i.split("**_**")
    dataofiteration.__delitem__(0)
    suma = 0
    print(f'Num elementos: {len(dataofiteration)}')
    for j in dataofiteration:
        cad2 = j.split("-")
        # print("time ",(float)(cad2[3].replace(" ","").split("=")[1]))
        suma += (float)(cad2[3].replace(" ","").split("=")[1])  
    suma /= len(dataofiteration)
    parallel_time.append(suma)
    speedup = parallel_time[0]/suma
    speedups.append(speedup)
    print(f'Speedup {item}: {speedup}')
    efficiencies.append(speedup/item)
    item += 1

xpoints = np.arange(1,threads)

print(len(speedups))
print(xpoints)
ypoints = np.array(speedups)
plt.title("Rendimiento")
plt.xlabel("#CPU's")
plt.ylabel("SPEEDUP")
plt.plot(xpoints, ypoints, label="SpeedUP")
plt.plot(xpoints, efficiencies, label="Efficiency")
plt.plot(xpoints, xpoints, label="Ideal speedup")
plt.legend()
plt.grid()
plt.show()
