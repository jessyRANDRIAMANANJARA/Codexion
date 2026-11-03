/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:53:52 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/02 23:59:38 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"

void	update_burnout_deadline(t_coder *coder)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	coder->burnout_deadline.tv_sec = current_time.tv_sec
		+ (coder->params.time_to_burnout / 1000);
	coder->burnout_deadline.tv_usec = current_time.tv_usec
		+ (coder->params.time_to_burnout % 1000) * 1000;
}

/* Attente par tranches de 1 ms pour pouvoir s'arrêter vite */
static void	ms_sleep(t_simulation *sim, int ms)
{
	int	elapsed;

	elapsed = 0;
	while (elapsed < ms && !is_stopped(sim))
	{
		usleep(1000);
		elapsed++;
	}
}

static bool	try_acquire_dongles(t_simulation *sim, t_coder *coder)
{
	while (!is_stopped(sim))
	{
		if (acquire_dongles(sim, coder))
			return (true);
		usleep(500);
	}
	return (false);
}

static void	perform_compilation(t_simulation *sim, t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	gettimeofday(&coder->last_compile_start, NULL);
	update_burnout_deadline(coder);
	pthread_mutex_unlock(&coder->mutex);
	log_event(sim, "is compiling", coder->id);
	ms_sleep(sim, sim->params.time_to_compile);
	pthread_mutex_lock(&coder->mutex);
	coder->compiles_count++;
	pthread_mutex_unlock(&coder->mutex);
}

static bool	compiles_done(t_simulation *sim, t_coder *coder)
{
	bool	done;

	pthread_mutex_lock(&coder->mutex);
	done = (coder->compiles_count >= sim->params.num_compiles_required);
	pthread_mutex_unlock(&coder->mutex);
	return (done);
}

void	*coder_routine(void *arg)
{
	t_simulation	*sim;
	t_coder			*coder;

	sim = &g_simulation;
	coder = &sim->coders[*(int *)arg];
	free(arg);
	pthread_mutex_lock(&coder->mutex);
	coder->running = true;
	pthread_mutex_unlock(&coder->mutex);
	while (!is_stopped(sim))
	{
		if (!try_acquire_dongles(sim, coder))
			break ;
		perform_compilation(sim, coder);
		release_dongles(sim, coder);
		if (compiles_done(sim, coder))
			break ;
		log_event(sim, "is debugging", coder->id);
		ms_sleep(sim, sim->params.time_to_debug);
		log_event(sim, "is refactoring", coder->id);
		ms_sleep(sim, sim->params.time_to_refactor);
	}
	pthread_mutex_lock(&coder->mutex);
	coder->running = false;
	pthread_mutex_unlock(&coder->mutex);
	return (NULL);
}
