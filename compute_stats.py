import pandas as pd
import itertools as it
import matplotlib.pyplot as plt

headers = ["Size", "Workers", "Completion Time", "Type"]

df = pd.read_csv("results/parallel.csv", names=headers)
df2 = pd.DataFrame(columns=["Size", "Workers", "Mean Completion Time", "Type"])

markers_list=['.','o','v','^','<','>']
greyscale=['0.15','0.35','0.55','0.75','0.90']

types=["chunks", "chunks_tp", "cyclic", "ff"]
sizes=[500,5000, 20000]
workers_list=[1,2,4,8,16,32]

param_grid = {
        "sizes": sizes,
        'workers': workers_list,
        "type": types,
}

keys = param_grid.keys()
combinations = it.product(*(param_grid[key] for key in keys))

param_list = list(combinations)

for index, elem in enumerate(param_list):
    size=elem[0]
    workers=elem[1]
    type=elem[2]

    temp_df = df.loc[(df['Size'] == size) & (df['Type'] == type) & (df['Workers'] == workers)]

    mean = temp_df["Completion Time"].mean()

    df2.loc[index] = [size, workers, mean, type]

df2.to_csv("./results/mean_completion_times_parallel.csv")

for elem in sizes:
    for i, type in enumerate(types):
        temp_df2 = df2.loc[(df2['Size'] == elem) & (df2['Type'] == type)]
        plt.plot(temp_df2["Workers"], temp_df2["Mean Completion Time"], label=type, marker=markers_list[i],
             c=greyscale[i])
    plt.title("MATRIX SIZE " + str(elem))
    plt.legend()
    plt.xlabel("WORKERS")
    plt.ylabel("COMPLETION TIME")
    plt.grid()
    plt.savefig("./results/Completion Time Matrix Size "+str(elem)+".png")
    plt.show()

columns = ["Workers", "CHUNKS", "CHUNKS_TP", "CYCLIC", "FF"]


#### SCALABILITY DATAFRAME
scalability_df = pd.DataFrame(columns=columns)
for size in sizes:
    tpar1_list = df2.loc[(df2['Size'] == size) & (df2['Workers'] == 1)]["Mean Completion Time"]

    for i, worker in enumerate(workers_list):
        tparn_list = df2.loc[(df2['Size'] == size) & (df2['Workers'] == worker)]["Mean Completion Time"]

        scalability_list = []
        for tpar1, tparn in zip(tpar1_list, tparn_list):
            scalability_list.append(round((tpar1/tparn),3))

        scalability_df.loc[i] = [worker]+scalability_list

    scalability_df.to_csv(f"./results/scalability{size}.csv")


##### SPEEDUP DATAFRAME
sequential_df = pd.read_csv("./results/sequential.csv", names=["Size", "Completion Time"])
sequential_df2 = pd.DataFrame(columns=["Size", "Mean Completion Time"])

for i, size in enumerate(sizes):
    mean = sequential_df.loc[(sequential_df["Size"]==size)]["Completion Time"].mean()
    sequential_df2.loc[i] = [size, mean]

    k=1
    while (k<=32):
        print(f"Ideal completion time size {size} workers {k}: {mean/k}")


        k*=2

sequential_df2.to_csv("./results/mean_completion_times_sequential.csv")

speedup_df = pd.DataFrame(columns=columns)

for j, size in enumerate(sizes):
    tseq_list = sequential_df2["Mean Completion Time"]

    for i, worker in enumerate(workers_list):
        tparn_list = df2.loc[(df2['Size'] == size) & (df2['Workers'] == worker)]["Mean Completion Time"]

        speedup_list = []
        for tparn in tparn_list:
            speedup_list.append(round((tseq_list[j]/tparn),3))

        speedup_df.loc[i] = [worker]+speedup_list

    speedup_df.to_csv(f"./results/speedup{size}.csv")

##### EFFICIENCY DATAFRAME
for j, size in enumerate(sizes):
    speedup_df = pd.read_csv(f"./results/speedup{size}.csv")
    efficiency_df = pd.DataFrame(columns=columns)

    for index, row in speedup_df.iterrows():

        efficiency_df.loc[index] = [row["Workers"], round((row["CHUNKS"]/row["Workers"]),3),
                                    round((row["CHUNKS_TP"] / row["Workers"]),3),  round((row["CYCLIC"] / row["Workers"]),3),
                                    round((row["FF"] / row["Workers"]),3)]

    efficiency_df.to_csv(f"./results/efficiency{size}.csv")
