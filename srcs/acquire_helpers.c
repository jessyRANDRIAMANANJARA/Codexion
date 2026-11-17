/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 18:00:00 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 18:00:00 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	try_acquire_dongles_simple(t_simulation *sim, t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % sim->params.num_coders;
	
	pthread_mutex_lock(&sim->simulation_mutex);
	
	int current_time = get_current_timestamp(sim);
	if (sim->dongles[left].available && sim->dongles[right].available &&
		is_dongle_available(sim, left, current_time) && 
		is_dongle_available(sim, right, current_time))
	{
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

bool	try_acquire_dongles_with_scheduler(t_simulation *sim, t_coder *coder)
{
	int	attempts;

	if (sim->params.num_coders == 1)
		return (false);
	
	attempts = 0;
	while (attempts < 100 && !is_stopped(sim))
	{
		if (try_acquire_dongles_simple(sim, coder))
			return (true);
		
		pthread_mutex_lock(&sim->simulation_mutex);
		pthread_cond_wait(&sim->dongles_available, &sim->simulation_mutex);
		pthread_mutex_unlock(&sim->simulation_mutex);
		
		if (!is_stopped(sim))
			usleep(1000);
		
		attempts++;
	}
	
	return (false);
}