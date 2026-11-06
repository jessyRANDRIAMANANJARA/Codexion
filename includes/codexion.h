/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrandri2 <hrandri2@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/31 08:09:23 by hrandri2          #+#    #+#             */
/*   Updated: 2026/11/06 16:59:01 by hrandri2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>
# include <limits.h>
# include <string.h>

struct	s_simulation;

typedef struct s_dongle
{
	int					id;
	bool				available;
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	struct timeval		release_time;
}					t_dongle;

typedef struct s_params
{
	int					num_coders;
	int					time_to_burnout;
	int					time_to_compile;
	int					time_to_debug;
	int					time_to_refactor;
	int					num_compiles_required;
	int					dongle_cooldown;
	char				*scheduler;
}					t_params;

typedef struct s_coder
{
	int					id;
	int					compiles_count;
	struct timeval		last_compile_start;
	struct timeval		burnout_deadline;
	pthread_t			thread;
	bool				alive;
	bool				running;
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	t_params			params;
	struct s_simulation	*sim;
}					t_coder;

typedef struct s_monitor
{
	pthread_t			thread;
	bool				running;
	bool				simulation_ended;
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
}					t_monitor;

typedef struct s_simulation
{
	t_params			params;
	t_dongle			*dongles;
	t_coder				*coders;
	t_monitor			monitor;
	pthread_mutex_t		log_mutex;
	pthread_mutex_t		simulation_mutex;
	bool				should_stop;
	struct timeval		start_time;
}					t_simulation;

// Priority queue for scheduling
typedef struct s_pq_node
{
	int					priority;
	int					coder_id;
	struct s_pq_node	*next;
}					t_pq_node;

typedef struct s_priority_queue
{
	t_pq_node			*head;
	pthread_mutex_t		mutex;
}					t_priority_queue;

// Function prototypes
int					parsing_input(
						t_simulation *sim, t_params *params, char **av
						);
void				*coder_routine(void *arg);
void				*monitor_routine(void *arg);
int					init_simulation(t_simulation *sim);
int					init_dongles(t_simulation *sim);
int					init_coders(t_simulation *sim);
void				init_monitor(t_simulation *sim);
int					abort_threads(t_simulation *sim, int started);
void				cleanup_simulation(t_simulation *sim);
void				get_time_from_start(t_simulation *sim);
void				log_event(
						t_simulation *sim, const char *event, int coder_id
						);
void				release_dongle(t_simulation *sim, int dongle_id);
bool				acquire_dongles(t_simulation *sim, t_coder *coder);
void				release_dongles(t_simulation *sim, t_coder *coder);
bool				is_stopped(t_simulation *sim);
void				*heap_insert(
						t_priority_queue *pq, t_simulation *sim,
						int priority, int coder_id
						);
int					heap_extract(
						t_priority_queue *pq, int *priority, int *coder_id
						);
void				heap_cleanup(t_priority_queue *pq);
int					get_current_timestamp(t_simulation *sim);
void				timestamp_to_timeval(t_simulation *sim, int timestamp,
						struct timeval *tv);
int					timeval_diff_ms(
						const struct timeval *start, const struct timeval *end
						);
bool				is_dongle_available(
						t_simulation *sim, int dongle_id, int current_time
						);
void				update_burnout_deadline(t_coder *coder);

#endif