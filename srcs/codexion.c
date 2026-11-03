/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:01 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/03 00:01:19 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_simulation	g_simulation;

int	main(int ac, char **av)
{
	int	i;

	if (ac != 9)
	{
		write(2, "Error: Invalid number of arguments\n", 35);
		return (1);
	}
	if (parsing_input(&g_simulation, &g_simulation.params, av + 1))
		return (1);
	get_time_from_start();
	if (init_simulation(&g_simulation))
	{
		cleanup_simulation(&g_simulation);
		return (1);
	}
	i = 0;
	while (i < g_simulation.params.num_coders)
		pthread_join(g_simulation.coders[i++].thread, NULL);
	pthread_join(g_simulation.monitor.thread, NULL);
	cleanup_simulation(&g_simulation);
	return (0);
}
