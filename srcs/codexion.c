/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:01 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 14:05:55 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	int				i;
	t_simulation	sim;

	memset(&sim, 0, sizeof(sim));
	if (ac != 9)
	{
		write(2, "Error: Invalid number of arguments\n", 35);
		return (1);
	}
	if (parsing_input(&sim, &sim.params, av + 1))
		return (1);
	get_time_from_start(&sim);
	if (init_simulation(&sim))
	{
		cleanup_simulation(&sim);
		return (1);
	}
	i = 0;
	while (i < sim.params.num_coders)
		pthread_join(sim.coders[i++].thread, NULL);
	pthread_join(sim.monitor.thread, NULL);
	cleanup_simulation(&sim);
	return (0);
}
