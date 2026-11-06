/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_routine.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 15:27:01 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 15:36:42 by hrandri2         ###   ########.fr       */
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
