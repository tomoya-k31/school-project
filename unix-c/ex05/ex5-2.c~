#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <limits.h>

int main(int argc, char *argv[])
{
  typedef struct {int x,y;struct point_t *next;} point_t;
  point_t *head = NULL,tmp;
  while(scanf("%d %d",&tmp.x,&tmp.y) == 2){
    point_t *t = malloc(sizeof(point_t));
    if (!t) {perror("malloc"); break;}
    *t = tmp;
    t->next = head;
    head = t;
  }
  while(head){
    point_t *t;
    printf("(%d,%d)\n",head->x,head->y);
    t = head->next;
    free(head);
    head = t;
  }
}
