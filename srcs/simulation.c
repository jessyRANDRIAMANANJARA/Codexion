/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:55:15 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 16:20:07 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	start_threads(t_simulation *sim)
{
	int	i;

	if (pthread_create(&sim->monitor.thread, NULL, monitor_routine, sim))
		return (1);
	i = 0;
	while (i < sim->params.num_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]))
			return (abort_threads(sim, i));
		i++;
	}
	return (0);
}

int	init_simulation(t_simulation *sim)
{
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->simulation_mutex, NULL);
	init_monitor(sim);
	if (init_dongles(sim) != 0 || init_coders(sim) != 0)
		return (1);
	return (start_threads(sim));
}
