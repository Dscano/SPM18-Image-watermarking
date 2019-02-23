import math
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.colors as color

def column(matrix, i):
    return [row[i] for row in matrix]

def idea(a, i):
    return a/i

def truncate(f, n):
    return math.floor(f * 10 ** n) / 10 ** n

def calc_speedup(val, j):
    return (Tseq[j])/val

def calc_efficiency(val, i, j):
    return  speedup[i][j]/par_workers[i]

def calc_scalability(i, j):
    return T_w[0][j]/T_w[i][j]

def ff_calc_scalability(i, j):
    return ff_T_w[0][j]/ff_T_w[i][j]

def ff_calc_efficiency(val, i, j):
    return  ff_speedup[i][j]/par_workers[i]

def ff_calc_scalability_one(i, j):
    return ff_T_w_one[0][j]/ff_T_w_one[i][j]

def ff_calc_efficiency_one(val, i, j):
    return  ff_speedup_one[i][j]/par_workers[i]
		
Tseq = [ 43400.8, 59527.8 ]

par_workers = [1,2,4,8,16,32,64]

w, h = 2, 7;
##w1 = 4

T_w = [[63264.2	, 114814.2],[45763.8, 79056.2],[24309.2, 34220.2],[17679.4, 26586.4]
    , [16716.2, 24951.2], [20611, 26846.8], [16733.6, 27449.75]]

ff_T_w = [[28086.6, 62913.4],[20002.8, 39545.6],[10963.4, 16147.4],[7883.8, 12977.6]
    , [19340.2, 25604], [25997, 28961.6], [27872.4, 37033.8]]

ff_T_w_one= [[61716.4, 117434 ],[33461.2, 60588],[19819.25,33115.8],[17221, 21011.75]
    , [18386, 19720], [19054.6, 20887.25], [21507.8	, 25748.8]]
'''
T = []
Tb = []

for i in range(len(par_workers)):

     T.append(Tseq[0]/par_workers[i])

for i in range(len(par_workers)):

     Tb.append(Tseq[1]/par_workers[i])

print("ideal compleion", T)
print("ideal compleion", Tb)
'''
efficiency = [[ 0 for x in range(w)] for y in range(h)] 
ff_efficiency = [[ 0 for x in range(w)] for y in range(h)] 
scalability = [[ 0 for x in range(w)] for y in range(h)] 
ff_scalability = [[ 0 for x in range(w)] for y in range(h)] 
speedup = [[ 0 for x in range(w)] for y in range(h)] 
ff_speedup = [[ 0 for x in range(w)] for y in range(h)]
ff_efficiency_one = [[ 0 for x in range(w)] for y in range(h)]
ff_speedup_one = [[ 0 for x in range(w)] for y in range(h)]
ff_scalability_one = [[ 0 for x in range(w)] for y in range(h)]



j = 0
i = 0
for x in T_w:
    for j in range(0, 2):

        speedup[i][j] = calc_speedup(T_w[i][j], j)
        speedup[i][j] = truncate(speedup[i][j], 3)
    i += 1


print("speedup",speedup)

j = 0
i = 0
for x in T_w:
    for j in range(0, 2):
        scalability[i][j] = calc_scalability(i, j)
        scalability[i][j] = truncate(scalability[i][j], 3)
    i += 1

print("scalability",scalability)

j = 0
i = 0
for x in speedup:
    for j in range(0, 2):
        efficiency[i][j] = calc_efficiency(speedup[i][j], i, j)
        efficiency[i][j] = truncate(efficiency[i][j], 3)
    i += 1

print("efficency",efficiency)

j = 0
i = 0
for x in ff_T_w:
    for j in range(0, 2):
        ff_speedup[i][j] = calc_speedup(ff_T_w[i][j], j)
        ff_speedup[i][j] = truncate(ff_speedup[i][j], 3)
    i += 1

print("ff_speedup", ff_speedup)

j = 0
i = 0
for x in ff_T_w:
    for j in range(0, 2):
        ff_scalability[i][j] = ff_calc_scalability(i, j)
        ff_scalability[i][j] = truncate(ff_scalability[i][j], 3)
    i += 1

print("ff_scalability", ff_scalability)

j = 0
i = 0
for x in ff_speedup:
    for j in range(0, 2):
        ff_efficiency[i][j] = ff_calc_efficiency(ff_speedup[i][j], i, j)
        ff_efficiency[i][j] = truncate(ff_efficiency[i][j], 3)
    i += 1

print("ff_efficency", ff_efficiency)

j = 0
i = 0
for x in ff_T_w_one:
    for j in range(0, 2):
        ff_speedup_one[i][j] = calc_speedup(ff_T_w_one[i][j], j)
        ff_speedup_one[i][j] = truncate(ff_speedup_one[i][j], 3)
    i += 1

print("ff_speedup_one", ff_speedup_one)

j = 0
i = 0
for x in ff_T_w_one:
    for j in range(0, 2):
        ff_scalability_one[i][j] = ff_calc_scalability_one(i, j)
        ff_scalability_one[i][j] = truncate(ff_scalability_one[i][j], 3)
    i += 1

print("ff_scalability_one", ff_scalability_one)

j = 0
i = 0
for x in ff_speedup_one:
    for j in range(0, 2):
        ff_efficiency_one[i][j] = ff_calc_efficiency_one(ff_speedup_one[i][j], i, j)
        ff_efficiency_one[i][j] = truncate(ff_efficiency_one[i][j], 3)
    i += 1

print("ff_efficency_one", ff_efficiency_one)


####TC_1
#plt.plot(   par_workers, T,               'k--',      linewidth=1, label="Ideal Completion")
plt.plot(   par_workers, column(T_w, 0),        'ro--',     linewidth=1, label="C++ 800x600" )
plt.plot(   par_workers, column(ff_T_w, 0),     'b^-',      linewidth=1, label="FF 800x600" )
plt.plot(   par_workers, column(ff_T_w_one, 0),  'c|-',   linewidth=1, label="FF-ONE 800x600" )

plt.legend()

plt.axis([1,64, 0, max(column(ff_T_w, 0))])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Time (ms)')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()

####TC_2
#plt.plot(   par_workers, Tb,               'k--',      linewidth=1, label="Ideal Completion")
plt.plot(   par_workers, column(T_w, 1),        'ro--',     linewidth=1, label="C++ 1600x1200" )
plt.plot(   par_workers, column(ff_T_w, 1),     'b^-',      linewidth=1, label="FF 1600x1200" )
plt.plot(   par_workers, column(ff_T_w_one, 1), 'c|-',      linewidth=1, label="FF-ONE 1600x1200" )

plt.legend()

plt.axis([1,64, 0, max(column(ff_T_w, 1))])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Time (ms)')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()


# ######SP1
plt.plot(   par_workers, par_workers,               'k--',      linewidth=2, label="Ideal speedup")
plt.plot(   par_workers, column(speedup, 0),        'ro--',     linewidth=1, label="C++ 800x600" )
plt.plot(   par_workers, column(ff_speedup, 0),     'b^-',      linewidth=1, label="FF 800x600" )
plt.plot(   par_workers, column(ff_speedup_one, 0),  'c|-',     linewidth=1, label="FF-ONE 800x600" )

plt.legend()

plt.axis([1,64, 1, 8])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Speedup')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()

# ######SP2
plt.plot(   par_workers, par_workers,               'k--',      linewidth=2, label="Ideal speedup")
plt.plot(   par_workers, column(speedup, 1),        'ro--',     linewidth=1, label="C++ 1600x1200" )
plt.plot(   par_workers, column(ff_speedup, 1),     'b^-',      linewidth=1, label="FF 1600x1200" )
plt.plot(   par_workers, column(ff_speedup_one, 1),  'c|-',     linewidth=1, label="FF-ONE 1600x1200" )

plt.legend()

plt.axis([1,64, 1, 11])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Speedup')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()


# ######SCALABILITY 1
# plt.plot(   par_workers, ([1] * 11),                  'k--',      linewidth=2, label="Ideal efficiency")
plt.plot(   par_workers, column(scalability, 0),        'ro--',     linewidth=1, label="C++ 800x600" )
plt.plot(   par_workers, column(ff_scalability, 0),     'b^-',      linewidth=1, label="FF 800x600" )
plt.plot(   par_workers, column(ff_scalability_one, 0),  'c|-',     linewidth=1, label="FF-ONE 800x600" )
plt.legend()

plt.axis([1,64, 0, 8])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Scalability')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()

#######SCALABILITY 2
# plt.plot(   par_workers, ([1] * 11),                  'k--',      linewidth=2, label="Ideal efficiency")
plt.plot(   par_workers, column(scalability, 1),        'ro--',     linewidth=1, label="C++ 1600x1200" )
plt.plot(   par_workers, column(ff_scalability, 1),     'b^-',      linewidth=1, label="FF 1600x1200" )
plt.plot(   par_workers, column(ff_scalability_one, 1), 'c|-',      linewidth=1, label="FF-ONE 1600x1200")


plt.legend()

plt.axis([1,64, 0,8 ])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Scalability')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()

# ######EFF1
plt.plot(   par_workers, column(efficiency, 0),       'ro--',     linewidth=1, label="C++ 800x600" )
plt.plot(   par_workers, column(ff_efficiency, 0),     'b^-',      linewidth=1, label="FF 800x600" )
plt.plot(   par_workers, column(ff_efficiency_one, 0),  'c|-',     linewidth=1, label="FF-ONE 800x600" )
plt.legend()

plt.axis([1,64, 0, 2])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Efficiency')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()

# #######EFF2
#plt.plot(   par_workers, ([1] * 7),                  'k--',        linewidth=2, label="Ideal efficiency")
plt.plot(   par_workers, column(efficiency, 1),        'ro--',     linewidth=1, label="C++ 1600x1200" )
plt.plot(   par_workers, column(ff_efficiency, 1),     'b^-',      linewidth=1, label="FF 1600x1200" )
plt.plot(   par_workers, column(ff_efficiency_one, 1),  'c|-',     linewidth=1, label="FF-ONE 1600x1200" )

plt.legend()

plt.axis([1,64, 0, 2])
plt.xticks(2**np.arange(7, dtype = np.uint64)[::-1])

plt.ylabel('Efficiency')
plt.xlabel('Parallelism Degree')
plt.grid(True)
plt.show()







