/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:39 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/02 23:55:03 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <unistd.h>
#include <string.h>

int	is_valid_number(const char *str)
{
	if (!str || !*str)
		return (0);
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (0);
		str++;
	}
	return (1);
}

int	parsing_input(t_simulation *sim, t_params *params, char **av)
{
	// Parse number of coders
	if (!is_valid_number(av[0]))
	{
		write(2, "Error: Invalid number of coders\n", 31);
		return (1);
	}
	params->num_coders = atoi(av[0]);
	if (params->num_coders <= 0)
	{
		write(2, "Error: Number of coders must be positive\n", 41);
		return (1);
	}

	// Parse time to burnout
	if (!is_valid_number(av[1]))
	{
		write(2, "Error: Invalid time to burnout\n", 31);
		return (1);
	}
	params->time_to_burnout = atoi(av[1]);
	if (params->time_to_burnout <= 0)
	{
		write(2, "Error: Time to burnout must be positive\n", 40);
		return (1);
	}

	// Parse time to compile
	if (!is_valid_number(av[2]))
	{
		write(2, "Error: Invalid time to compile\n", 30);
		return (1);
	}
	params->time_to_compile = atoi(av[2]);
	if (params->time_to_compile <= 0)
	{
		write(2, "Error: Time to compile must be positive\n", 39);
		return (1);
	}

	// Parse time to debug
	if (!is_valid_number(av[3]))
	{
		write(2, "Error: Invalid time to debug\n", 29);
		return (1);
	}
	params->time_to_debug = atoi(av[3]);
	if (params->time_to_debug <= 0)
	{
		write(2, "Error: Time to debug must be positive\n", 38);
		return (1);
	}

	// Parse time to refactor
	if (!is_valid_number(av[4]))
	{
		write(2, "Error: Invalid time to refactor\n", 31);
		return (1);
	}
	params->time_to_refactor = atoi(av[4]);
	if (params->time_to_refactor <= 0)
	{
		write(2, "Error: Time to refactor must be positive\n", 40);
		return (1);
	}

	// Parse number of compiles required
	if (!is_valid_number(av[5]))
	{
		write(2, "Error: Invalid number of compiles required\n", 43);
		return (1);
	}
	params->num_compiles_required = atoi(av[5]);
	if (params->num_compiles_required <= 0)
	{
		write(2, "Error: Number of compiles required must be positive\n", 51);
		return (1);
	}

	// Parse dongle cooldown
	if (!is_valid_number(av[6]))
	{
		write(2, "Error: Invalid dongle cooldown\n", 30);
		return (1);
	}
	params->dongle_cooldown = atoi(av[6]);
	if (params->dongle_cooldown < 0)
	{
		write(2, "Error: Dongle cooldown cannot be negative\n", 41);
		return (1);
	}

	// Parse scheduler
	params->scheduler = av[7];
	if (strcmp(params->scheduler, "fifo") != 0 && strcmp(params->scheduler, "edf") != 0)
	{
		write(2, "Error: Scheduler must be 'fifo' or 'edf'\n", 41);
		return (1);
	}

	// Initialize simulation parameters
	sim->params = *params;
	sim->should_stop = false;
	return (0);
}