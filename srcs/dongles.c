/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:07 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/02 23:54:19 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	take_one(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = false;
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_simulation *sim, int dongle_id)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = true;
	gettimeofday(&dongle->release_time, NULL);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

/* Prend les deux dongles de façon atomique (tout ou rien) : évite le deadlock */
bool	acquire_dongles(t_simulation *sim, t_coder *coder)
{
	int	left;
	int	right;
	int	now;

	left = coder->id - 1;
	right = coder->id % sim->params.num_coders;
	if (left == right)
		return (false);
	pthread_mutex_lock(&sim->simulation_mutex);
	now = get_current_timestamp();
	if (sim->should_stop || !is_dongle_available(sim, left, now)
		|| !is_dongle_available(sim, right, now))
	{
		pthread_mutex_unlock(&sim->simulation_mutex);
		return (false);
	}
	take_one(&sim->dongles[left]);
	take_one(&sim->dongles[right]);
	pthread_mutex_unlock(&sim->simulation_mutex);
	log_event(sim, "has taken a dongle", coder->id);
	log_event(sim, "has taken a dongle", coder->id);
	return (true);
}

void	release_dongles(t_simulation *sim, t_coder *coder)
{
	release_dongle(sim, coder->id - 1);
	release_dongle(sim, coder->id % sim->params.num_coders);
}
