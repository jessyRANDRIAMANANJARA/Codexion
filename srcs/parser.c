/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/02 23:54:39 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 17:57:19 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

static int	print_error(const char *name, const char *msg)
{
	write(2, "Error: ", 7);
	write(2, name, strlen(name));
	write(2, msg, strlen(msg));
	return (1);
}

static int	parse_field(const char *arg, const char *name, int *dst, int min)
{
	long	value;
	int		i;

	if (!is_valid_number(arg))
		return (print_error(name, " must be a valid integer\n"));
	value = 0;
	i = 0;
	while (arg[i] && value <= INT_MAX)
	{
		value = value * 10 + (arg[i] - '0');
		i++;
	}
	if (value > INT_MAX)
		return (print_error(name, " is too large\n"));
	if (value < min)
		return (print_error(name, " must be positive\n"));
	*dst = (int)value;
	return (0);
}

static int	parse_numbers(t_params *p, char **av)
{
	if (parse_field(av[0], "number_of_coders", &p->num_coders, 1))
		return (1);
	if (parse_field(av[1], "time_to_burnout", &p->time_to_burnout, 1))
		return (1);
	if (parse_field(av[2], "time_to_compile", &p->time_to_compile, 1))
		return (1);
	if (parse_field(av[3], "time_to_debug", &p->time_to_debug, 1))
		return (1);
	if (parse_field(av[4], "time_to_refactor", &p->time_to_refactor, 1))
		return (1);
	if (parse_field(av[5], "number_of_compiles_required",
			&p->num_compiles_required, 1))
		return (1);
	if (parse_field(av[6], "dongle_cooldown", &p->dongle_cooldown, 1))
		return (1);
	return (0);
}

int	parsing_input(t_simulation *sim, t_params *params, char **av)
{
	if (parse_numbers(params, av))
		return (1);
	params->scheduler = av[7];
	if (strcmp(av[7], "fifo") != 0 && strcmp(av[7], "edf") != 0)
		return (print_error("scheduler", " must be fifo or edf\n"));
	sim->params = *params;
	sim->should_stop = false;
	return (0);
}
