/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:53:52 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 17:00:37 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

	coder = (t_coder *)arg;
	sim = coder->sim;
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
