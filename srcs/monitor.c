/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:33 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 16:47:28 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static bool	check_coder_burnout(t_simulation *sim, t_coder *coder,
	int current_time)
{
	struct timeval	current_tv;

	timestamp_to_timeval(sim, current_time, &current_tv);
	return (timeval_diff_ms(&coder->last_compile_start, &current_tv)
		>= sim->params.time_to_burnout);
}

static void	set_stop(t_simulation *sim)
{
	pthread_mutex_lock(&sim->simulation_mutex);
	sim->should_stop = true;
	pthread_mutex_unlock(&sim->simulation_mutex);
}

static int	scan_coders(t_simulation *sim, bool *all_done)
{
	int		i;
	int		now;
	int		found;
	t_coder	*c;

	now = get_current_timestamp(sim);
	found = -1;
	*all_done = true;
	i = 0;
	while (i < sim->params.num_coders && found == -1)
	{
		c = &sim->coders[i];
		pthread_mutex_lock(&c->mutex);
		if (c->compiles_count < sim->params.num_compiles_required)
		{
			*all_done = false;
			if (check_coder_burnout(sim, c, now))
				found = i;
		}
		pthread_mutex_unlock(&c->mutex);
		i++;
	}
	return (found);
}

static void	end_monitor(t_simulation *sim)
{
	pthread_mutex_lock(&sim->monitor.mutex);
	sim->monitor.running = false;
	sim->monitor.simulation_ended = true;
	pthread_mutex_unlock(&sim->monitor.mutex);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int				burnout_coder;
	bool			all_done;

	sim = (t_simulation *)arg;
	while (!is_stopped(sim))
	{
		burnout_coder = scan_coders(sim, &all_done);
		if (burnout_coder != -1)
		{
			set_stop(sim);
			log_event(sim, "burned out", burnout_coder + 1);
			break ;
		}
		if (all_done)
		{
			set_stop(sim);
			break ;
		}
		usleep(1000);
	}
	end_monitor(sim);
	return (NULL);
}
