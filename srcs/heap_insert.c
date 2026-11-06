/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_insert.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 15:40:26 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 16:04:50 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_pq_node	*create_pq_node(int priority, int coder_id)
{
	t_pq_node	*node;

	node = malloc(sizeof(t_pq_node));
	if (!node)
		return (NULL);
	node->priority = priority;
	node->coder_id = coder_id;
	node->next = NULL;
	return (node);
}

static void	insert_sorted(t_priority_queue *pq, t_pq_node *node)
{
	t_pq_node	*current;
	t_pq_node	*prev;

	prev = NULL;
	current = pq->head;
	while (current && current->priority <= node->priority)
	{
		prev = current;
		current = current->next;
	}
	if (!prev)
	{
		node->next = pq->head;
		pq->head = node;
	}
	else
	{
		prev->next = node;
		node->next = current;
	}
}

void	*heap_insert(t_priority_queue *pq, t_simulation *sim,
		int priority, int coder_id)
{
	t_pq_node	*new_node;
	t_pq_node	*current;

	pthread_mutex_lock(&pq->mutex);
	new_node = create_pq_node(priority, coder_id);
	if (!new_node)
	{
		pthread_mutex_unlock(&pq->mutex);
		return (NULL);
	}
	if (!pq->head)
		pq->head = new_node;
	else if (strcmp(sim->params.scheduler, "fifo") == 0)
	{
		current = pq->head;
		while (current->next)
			current = current->next;
		current->next = new_node;
	}
	else
		insert_sorted(pq, new_node);
	pthread_mutex_unlock(&pq->mutex);
	return (new_node);
}
