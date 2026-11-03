/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:13 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/02 23:58:47 by hrandri2         ###   ########.fr       */
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

void	*heap_insert(t_priority_queue *pq, t_simulation *sim, int priority, int coder_id)
{
	t_pq_node	*new_node;
	t_pq_node	*current;
	t_pq_node	*prev;

	pthread_mutex_lock(&pq->mutex);
	
	new_node = create_pq_node(priority, coder_id);
	if (!new_node)
	{
		pthread_mutex_unlock(&pq->mutex);
		return (NULL);
	}

	if (!pq->head)
	{
		pq->head = new_node;
	}
	else
	{
		if (strcmp(sim->params.scheduler, "fifo") == 0)
		{
			// FIFO: insert at end
			current = pq->head;
			while (current->next)
				current = current->next;
			current->next = new_node;
		}
		else
		{
			// EDF: insert in priority order (lower deadline = higher priority)
			prev = NULL;
			current = pq->head;
			while (current && current->priority <= priority)
			{
				prev = current;
				current = current->next;
			}
			if (!prev)
			{
				new_node->next = pq->head;
				pq->head = new_node;
			}
			else
			{
				prev->next = new_node;
				new_node->next = current;
			}
		}
	}
	pthread_mutex_unlock(&pq->mutex);
	return (new_node);
}

int	heap_extract(t_priority_queue *pq, int *priority, int *coder_id)
{
	t_pq_node	*temp;

	pthread_mutex_lock(&pq->mutex);
	if (!pq->head)
	{
		pthread_mutex_unlock(&pq->mutex);
		return (0);
	}

	temp = pq->head;
	*priority = temp->priority;
	*coder_id = temp->coder_id;
	pq->head = pq->head->next;
	free(temp);
	pthread_mutex_unlock(&pq->mutex);
	return (1);
}

void	heap_cleanup(t_priority_queue *pq)
{
	t_pq_node	*current;
	t_pq_node	*next;

	pthread_mutex_lock(&pq->mutex);
	current = pq->head;
	while (current)
	{
		next = current->next;
		free(current);
		current = next;
	}
	pq->head = NULL;
	pthread_mutex_unlock(&pq->mutex);
}