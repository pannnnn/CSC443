import os
import matplotlib.pyplot as plt

def plot_cvs2colstore_chart():
    f = open("csv2colstore_data.txt").readlines()
    header = f.pop(0)

    x = []
    y = []
    for line in f:
        line = line.split()
        x.append(int(line[0]))
        y.append(float(line[1]) * 1000)

    plt.figure()
    plt.plot(x, y, 'bo-')
    plt.title('Page Size vs. Performance (cvs2colstore)')
    plt.xlabel('Page Size (bytes)')
    plt.ylabel('Time (ms)')
    plt.savefig('cvs2colstore Performance.png')
    plt.close()

def plot_select2_chart():
    f = open("select2_data.txt").readlines()
    header = f.pop(0)

    x = []
    y = []
    for line in f:
        line = line.split()
        x.append(int(line[0]))
        y.append(float(line[1]) * 1000)

    plt.figure()
    plt.plot(x, y, 'bo-')
    plt.title('Page Size vs. Performance (Select2)')
    plt.xlabel('Page Size (bytes)')
    plt.ylabel('Time (ms)')
    plt.savefig('Select2 Performance.png')
    plt.close()

def plot_select3_chart():
    f = open("select3_data.txt").readlines()
    header = f.pop(0)

    x = []
    y = []
    for line in f:
        line = line.split()
        x.append(int(line[0]))
        y.append(float(line[1]) * 1000)

    plt.figure()
    plt.plot(x, y, 'bo-')
    plt.title('Page Size vs. Performance (Select3)')
    plt.xlabel('Page Size (bytes)')
    plt.ylabel('Time (ms)')
    plt.savefig('Select3 Performance.png')
    plt.close()

def main():
    # if not os.path.isfile("read_data.txt") or not os.path.isfile("write_data.txt"):
    #     print("Must run python program 'experiment_block_size.py' in the current directory!")
    #     exit()

    plot_cvs2colstore_chart()
    plot_select2_chart()
    plot_select3_chart()
    
    

if __name__ == "__main__":
    main()