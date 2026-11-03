/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:55:35 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/02 23:55:36 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdio.h>

bool	is_stopped(t_simulation *sim)
{
	bool	stopped;

	pthread_mutex_lock(&sim->simulation_mutex);
	stopped = sim->should_stop;
	pthread_mutex_unlock(&sim->simulation_mutex);
	return (stopped);
}

/* Après l'arrêt, seul "burned out" est encore affiché */
void	log_event(t_simulation *sim, const char *event, int coder_id)
{
	char	buffer[256];
	int		len;

	pthread_mutex_lock(&sim->log_mutex);
	if (is_stopped(sim) && strcmp(event, "burned out") != 0)
	{
		pthread_mutex_unlock(&sim->log_mutex);
		return ;
	}
	len = snprintf(buffer, sizeof(buffer), "%d %d %s\n",
			get_current_timestamp(), coder_id, event);
	write(1, buffer, len);
	pthread_mutex_unlock(&sim->log_mutex);
}

void	cleanup_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	while (sim->dongles && i < sim->params.num_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
	i = 0;
	while (sim->coders && i < sim->params.num_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].mutex);
		pthread_cond_destroy(&sim->coders[i].cond);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
	pthread_mutex_destroy(&sim->monitor.mutex);
	pthread_cond_destroy(&sim->monitor.cond);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->simulation_mutex);
}

/* Un dongle est utilisable s'il est libre ET si son cooldown est écoulé */
bool	is_dongle_available(t_simulation *sim, int dongle_id, int current_time)
{
	struct timeval	current_tv;
	t_dongle		*dongle;
	bool			ok;

	timestamp_to_timeval(current_time, &current_tv);
	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->mutex);
	ok = dongle->available && timeval_diff_ms(&dongle->release_time,
			&current_tv) >= sim->params.dongle_cooldown;
	pthread_mutex_unlock(&dongle->mutex);
	return (ok);
}
