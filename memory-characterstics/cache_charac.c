#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>

char * plot(int x, long long int y) {
	char *points = malloc(20);
	sprintf(points, "%d,%lld;", x, y);
	return points;
}

double avg_elapsed(int size, int stride, int fd, int param) {
	printf("%d\n", stride);
	struct timespec before, after;
	long long int elapsed_nsecs;
	float a[size];
	int i = 0;
	for(i = 0; i < size; i++) {
		float x = (float)rand()/(float)(RAND_MAX/100);
		a[i] = x;
	}

	double avg_elapsed = 0.0;
	for(i = stride; i < size; i += stride) {
		clock_gettime(CLOCK_REALTIME, &before);
		double c = a[i] + a[i-1];
		a[i-stride] = c;
		printf("%lf\n", a[i-1]);
		clock_gettime(CLOCK_REALTIME, &after);
		elapsed_nsecs = (after.tv_sec - before.tv_sec) * 1000000000 +
									(after.tv_nsec - before.tv_nsec);
		avg_elapsed += elapsed_nsecs;

	}

	char *result;
	if (param == 0) { // NvsAvgIterTime
		 avg_elapsed = avg_elapsed/size;
		 result = plot(size, avg_elapsed);
	 } else if (param == 1) { // svsAvgIterTime
		 avg_elapsed = avg_elapsed*stride/size;
		 result = plot(stride, avg_elapsed);
	 }
	write(fd, result, 20);
	return avg_elapsed;
}

int main(int argc, char *argv[]) {
	// Plot N vs Avg Iteration Time graph to see the size behaviour of cache
	int fd = open("./avg_time_iter.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);

	int i = 0;
	for(i = 0; i < 20; i++) {
		printf("%lf\n", avg_elapsed(2 << i, 1, fd, 0));
	}

	close(fd);

	// Plot N vs Avg Iteration Time graph to see the size behaviour of cache
	int fd1 = open("./avg_time_iter_stride.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);

	i = 0;
	for(i = 0; i < 15; i++) {
		printf("%lf\n", avg_elapsed(2 << 19, 2 << i, fd1, 1));
	}

	close(fd1);

	return 0;
}
