#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct list {
	int i;
	struct list *prev;
	struct list *next;
};

int insert_item(struct list *head, struct list *m)
{
	struct list *last;

	last = head;

	if (last == NULL) {
		return -1;
	}

	while(last->next != NULL)
		last = last->next;

	last->next = m;
	m->prev = last;
	return 0;
}

struct list *remove_item(struct list *head, int num)
{
	struct list *cur;
	struct list *prev;
	struct list *next;
	struct list *match;

	cur = prev = next = head;

	while (cur != NULL) {
		if (cur->i == num)
			break;
		else
			cur = cur->next;
	}

	if (cur == NULL) {
		return head;
	}

	prev = cur->prev;
	next = cur->next;
	
	if (prev != NULL) {
		prev->next = cur->next;
	}

	if (next != NULL) {
		next->prev = cur->prev;
	}

	if (cur == head) {
		return cur->next;
	} else {
		return head;
	}
}

struct list *init_item(void)
{
	struct list *l;
	l = malloc(sizeof(struct list));
	memset(l, 0, sizeof(struct list));
	return l;
}

int print_list(struct list *l)
{
	while(l != NULL) {
		printf("%d\n", l->i);
		l = l->next;
  	}
        return 0;
}

int main(void)
{
	int i;
	struct list *head;
	struct list *m;
	head = init_item();
	for (i = 1; i < 10; i++) {
		m = init_item();
		m->i = i;
		insert_item(head, m);
	}
	for (i = 0; i < 10; i+=3) {
		head = remove_item(head, i);
	}
	print_list(head);
	return 0;
}
