#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <sys/time.h>

typedef struct
{
  double x;
  double y;
} Point;

void point_print(const Point* const point);
void read_stdin(Point** points, uint32_t* size);
int cmp_x(const void* a, const void* b);
inline double dist(const Point* p1, const Point* p2);
void brute_force(Point* points, uint32_t count, Point** a, Point** b, double* distance);
void divide_conquer(Point* points, uint32_t count, Point* a, Point* b, double* distance);

void point_print(const Point* const point)
{
  printf("(%.9f, %.9f) ", point->x, point->y);
}

void read_stdin(Point** points, uint32_t* size)
{
  struct timeval t1;
  gettimeofday(&t1, NULL);
  int scanfed = 0;
  uint32_t count = 0;
  scanfed = scanf("%d\n", &count);
  printf("N: %d\n", count);
  *points = calloc(count, sizeof(Point));
  *size = count;

  uint32_t i = 0;
  do
  {
    Point p;
    scanfed = scanf("(%lf,%lf)\n", &p.x, &p.y);
    if (scanfed != 2) continue;
    (*points)[i] = p;
    i++;
  } while (i < count && scanfed != EOF);

  struct timeval t2;
  gettimeofday(&t2, NULL);
  printf("read_stdin: %ld ms\n", ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec)) / 1000);
}

int cmp_x(const void* a, const void* b)
{
  const Point* pa = (const Point*) a;
  double ax = (double) pa->x;
  const Point* pb = (const Point*)b;
  double bx = (double) pb->x;

  if ((ax - bx) < -DBL_EPSILON) return -1;
  else if ((ax - bx) > DBL_EPSILON) return 1;
  else return 0;
}

double dist(const Point* p1, const Point* p2)
{
  return (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y);
}

void brute_force(Point* points, uint32_t count, Point** a, Point** b, double* distance)
{
  for (uint32_t i = 0; i < count - 1; i++)
  {
    for (uint32_t j = i + 1; j < count; j++)
    {
      double d =  dist(&points[i], &points[j]);

      if (d < *distance)
      {
        *distance = d;
        *a = &points[i];
        *b = &points[j];
      }
    }
  }
}

void divide_conquer(Point* points, uint32_t count, Point* a, Point* b, double* distance)
{
  if (count < 3)
  {
    brute_force(points, count, &a, &b, distance);
    return;
  }

  uint32_t middle_index = count / 2;

  Point* best_a_left = a;
  Point* best_b_left = b;
  double distance_left = DBL_MAX;
  divide_conquer(points, middle_index, best_a_left, best_b_left, &distance_left);

  Point* best_a_right = a;
  Point* best_b_right = b;
  double distance_right = DBL_MAX;
  divide_conquer(points + middle_index, count - middle_index, best_a_right, best_b_right, &distance_right);

  if (distance_left < distance_right)
  {
    *a = *best_a_left;
    *b = *best_b_left;
    *distance = distance_left;
  } 
  else 
  {
    *a = *best_a_right;
    *b = *best_b_right;
    *distance = distance_right;
  }

  // Check split points
  double distance_split = DBL_MAX;
  Point* best_a_split = a;
  Point* best_b_split = b;

  for (uint32_t i = 0; i < middle_index; ++i)
  {
    for (uint32_t j = middle_index; j < count; ++j) 
    {
      double d = dist(&points[i], &points[j]);
      if (DBL_EPSILON < d && d < distance_split)
      {
        *best_a_split = points[i];
        *best_b_split = points[j];
        distance_split = d;
      }
    }
  }
  
  if (DBL_EPSILON < distance_split && distance_split < *distance)
  {
    *a = *best_a_split;
    *b = *best_b_split;
    *distance = distance_split;
  }
}

int main()
{
  Point* points = NULL;
  uint32_t count = 0;
  read_stdin(&points, &count);

  if (count <= 10000)
  {
    struct timeval t1;
    gettimeofday(&t1, NULL);

    double min_distance = DBL_MAX;
    Point* best1 = NULL;
    Point* best2 = NULL;
    brute_force(points, count, &best1, &best2, &min_distance);

    struct timeval t2;
    gettimeofday(&t2, NULL);
    printf("Search naive: %ld ms\n", ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec)) / 1000);
    printf("Distance %.9f\n", min_distance);
    point_print(best1);
    point_print(best2);
    printf("\n");
    printf("\n");
  }
  {
    struct timeval t1;
    gettimeofday(&t1, NULL);

    qsort(points, count, sizeof(Point), &cmp_x);
    
    double min_distance = DBL_MAX;
    Point* best1 = &points[0];
    Point* best2 = &points[1];

    divide_conquer(points, count, best1, best2, &min_distance);

    struct timeval t2;
    gettimeofday(&t2, NULL);
    
    printf("Search divide&conquer: %ld ms\n", ((t2.tv_sec * 1000000 + t2.tv_usec) - (t1.tv_sec * 1000000 + t1.tv_usec)) / 1000);
    printf("Distance %.9f\n", min_distance);
    point_print(best1);
    point_print(best2);
    printf("\n");
  }

  free(points);

  return 0;
}
