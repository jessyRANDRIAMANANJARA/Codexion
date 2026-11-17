/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:07 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/07 07:05:22 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"



void	release_dongle(t_simulation *sim, int dongle_id)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = true;
	gettimeofday(&dongle->release_time, NULL);
	dongle->release_timestamp = get_current_timestamp(sim);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
	
	// Notify all coders that dongles might be available
	pthread_cond_broadcast(&sim->dongles_available);
}

bool	acquire_dongles(t_simulation *sim, t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % sim->params.num_coders;
	
	// For one coder, it's impossible to acquire two different dongles
	if (sim->params.num_coders == 1)
		return (false);
	
	pthread_mutex_lock(&sim->simulation_mutex);
	
	// Check if dongles are available with cooldown consideration
	int current_time = get_current_timestamp(sim);
	if (sim->dongles[left].available && sim->dongles[right].available &&
		is_dongle_available(sim, left, current_time) && 
		is_dongle_available(sim, right, current_time))
	{
		// Acquire dongles atomically
		pthread_mutex_lock(&sim->dongles[left].mutex);
		pthread_mutex_lock(&sim->dongles[right].mutex);
		
		if (sim->dongles[left].available && sim->dongles[right].available)
		{
			sim->dongles[left].available = false;
			sim->dongles[right].available = false;
			pthread_mutex_unlock(&sim->dongles[left].mutex);
			pthread_mutex_unlock(&sim->dongles[right].mutex);
			pthread_mutex_unlock(&sim->simulation_mutex);
			
			log_event(sim, "has taken a dongle", coder->id);
			log_event(sim, "has taken a dongle", coder->id);
			return (true);
		}
		
		pthread_mutex_unlock(&sim->dongles[left].mutex);
		pthread_mutex_unlock(&sim->dongles[right].mutex);
	}
	
	pthread_mutex_unlock(&sim->simulation_mutex);
	return (false);
}

void	release_dongles(t_simulation *sim, t_coder *coder)
{
	release_dongle(sim, coder->id - 1);
	release_dongle(sim, coder->id % sim->params.num_coders);
}
