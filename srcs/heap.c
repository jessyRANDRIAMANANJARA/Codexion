/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:13 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 15:41:35 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
