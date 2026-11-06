/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:55:29 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 16:30:18 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	get_time_from_start(t_simulation *sim)
{
	gettimeofday(&sim->start_time, NULL);
}

int	get_current_timestamp(t_simulation *sim)
{
	struct timeval	current_time;

	gettimeofday(&current_time, NULL);
	return ((current_time.tv_sec - sim->start_time.tv_sec) * 1000
		+ (current_time.tv_usec - sim->start_time.tv_usec) / 1000);
}

void	timestamp_to_timeval(
	t_simulation *sim, int timestamp, struct timeval *tv
)
{
	tv->tv_sec = sim->start_time.tv_sec + timestamp / 1000;
	tv->tv_usec = sim->start_time.tv_usec + (timestamp % 1000) * 1000;
}

int	timeval_diff_ms(const struct timeval *start, const struct timeval *end)
{
	return ((end->tv_sec - start->tv_sec) * 1000
		+ (end->tv_usec - start->tv_usec) / 1000);
}

bool	is_time_expired(t_simulation *sim, const struct timeval *deadline,
	int current_time)
{
	struct timeval	deadline_tv;

	timestamp_to_timeval(sim, current_time, &deadline_tv);
	return (timeval_diff_ms(&deadline_tv, deadline) >= 0);
}
