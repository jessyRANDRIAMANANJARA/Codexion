/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:55:15 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/02 23:55:16 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_simulation *sim)
{
	int	i;

	sim->dongles = calloc(sim->params.num_coders, sizeof(t_dongle));
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < sim->params.num_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		sim->dongles[i].id = i;
		sim->dongles[i].available = true;
		gettimeofday(&sim->dongles[i].release_time, NULL);
		/* le cooldown ne doit pas bloquer la 1re prise */
		sim->dongles[i].release_time.tv_sec
			-= sim->params.dongle_cooldown / 1000 + 1;
		i++;
	}
	return (0);
}

static int	init_coders(t_simulation *sim)
{
	int	i;

	sim->coders = calloc(sim->params.num_coders, sizeof(t_coder));
	if (!sim->coders)
		return (1);
	i = 0;
	while (i < sim->params.num_coders)
	{
		pthread_mutex_init(&sim->coders[i].mutex, NULL);
		pthread_cond_init(&sim->coders[i].cond, NULL);
		sim->coders[i].id = i + 1;
		sim->coders[i].alive = true;
		sim->coders[i].params = sim->params;
		gettimeofday(&sim->coders[i].last_compile_start, NULL);
		update_burnout_deadline(&sim->coders[i]);
		i++;
	}
	return (0);
}

static void	init_monitor(t_simulation *sim)
{
	pthread_mutex_init(&sim->monitor.mutex, NULL);
	pthread_cond_init(&sim->monitor.cond, NULL);
	sim->monitor.running = false;
	sim->monitor.simulation_ended = false;
}

/* En cas d'échec : on stoppe et on joint ce qui a déjà démarré */
static int	abort_threads(t_simulation *sim, int started)
{
	int	i;

	pthread_mutex_lock(&sim->simulation_mutex);
	sim->should_stop = true;
	pthread_mutex_unlock(&sim->simulation_mutex);
	i = 0;
	while (i < started)
		pthread_join(sim->coders[i++].thread, NULL);
	pthread_join(sim->monitor.thread, NULL);
	return (1);
}

static int	start_threads(t_simulation *sim)
{
	int	*arg;
	int	i;

	if (pthread_create(&sim->monitor.thread, NULL, monitor_routine, sim))
		return (1);
	i = 0;
	while (i < sim->params.num_coders)
	{
		arg = malloc(sizeof(int));
		if (!arg)
			return (abort_threads(sim, i));
		*arg = i;
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine, arg))
		{
			free(arg);
			return (abort_threads(sim, i));
		}
		i++;
	}
	return (0);
}

/* Ne nettoie pas : main appelle cleanup_simulation (évite le double free) */
int	init_simulation(t_simulation *sim)
{
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->simulation_mutex, NULL);
	init_monitor(sim);
	if (init_dongles(sim) != 0 || init_coders(sim) != 0)
		return (1);
	return (start_threads(sim));
}
