import matplotlib.pyplot as plt

cache_size = "avg_time_iter.txt";
average_miss_delay = "avg_time_iter_stride.txt";

def read_file(path):
    fd = open(path, "r")
    x_points, y_points = [], []
    s = ""
    while fd:
        c = fd.read(1)
        if not c:
            break
        if c == ";":
            x, y = s.split(',')
            x_points.append(int(x))
            y_points.append(int(y))
            s = ""
        elif c == ',' or c.isdigit():
            s += c

    return x_points, y_points

x, y = read_file(cache_size)

plt.plot(x, y, 'ro')
plt.axis([0, 2**20, 0, 35000])
plt.show()

x, y = read_file(average_miss_delay)

plt.plot(x, y, 'ro')
plt.axis([0, 2**18, 0, 90000])
plt.show()
