/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaekim <chaekim@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/20 01:05:59 by chaekim           #+#    #+#             */
/*   Updated: 2022/01/21 18:01:12 by chaekim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	print(char *str)
{
	int		i;
	char	ch;

	i = 0;
	while (*(str + i))
	{
		ch = *(str + i);
		write(1, &ch, 1);
		i++;
	}
}

int	str_len(char *str)
{
	int		i;

	i = 0;
	while (*(str + i))
		i++;
	return (i);
}

void	dup2_stdin_stdout(int in, int out)
{
	if (dup2(in, STDIN_FILENO) == -1)
		{
			close(in);
			perror("dup2");
			exit(1);
		}
		close(in);
		if (dup2(out, STDOUT_FILENO) == -1)
		{
			close(out);
			perror("dup2");
			exit(1);
		}
		close(out);
}

void	ft_error()
{
	perror("Error");
	exit(1);
}

char	*find_path_in_envp(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i]);
		i++;
	}
	return (0);
}

char	*find_path(char *command, char **envp)
{
	char	**en_paths;
	char	*en_path;
	int	i;

	en_path = find_path_in_envp(envp);
	if (!en_path && !command)
		return (0);
	en_paths = ft_split(en_path + 5, ':');
	i = 0;
	while (en_paths[i])
	{
		en_path = ft_strjoin(en_paths[i], "/");
		en_path = ft_strjoin(en_path, command);
		if (access(en_path, F_OK) == 0)
			return (en_path);
		i++;
	}
	return (0);
}

int	main(int argc, char *argv[], char *envp[])
{
	char	*path;
	char	**command;
	int		fd_open_file;
	int		fd_output;
	pid_t	pid;
	int		fd[2];

	if (argc != 5)
		ft_error();
	if (pipe(fd) == -1)
		ft_error();
	pid = fork();
	if (pid > 0)
	{
		command = ft_split(argv[2], ' ');
		path = find_path(command[0], envp);
		printf("path: %s\n", path);
		if (!path)
		{
			perror("path");
			exit(1);
		}
		fd_open_file = open(argv[1], O_RDONLY);
		if (fd_open_file == -1)
			ft_error();
		close(fd[0]);
		if (dup2(fd_open_file, STDIN_FILENO) == -1)
		{
			close(fd_open_file);
			perror("dup2");
			exit(1);
		}
		close(fd_open_file);
		if (dup2(fd[1], STDOUT_FILENO) == -1)
		{
			close(fd[1]);
			perror("dup2");
			exit(1);
		}
		close(fd[1]);
		if (execve(path, command, envp) == -1)
			ft_error();
		if (waitpid(pid, NULL, WNOHANG) == -1)
			ft_error();
	}
	else if (pid == 0) //자식 프로세서
	{
		command = ft_split(argv[3], ' ');
		path = find_path(command[0], envp);
		if (!path)
		{
			perror("command");
			exit(1);
		}
		fd_output = open(argv[4], O_CREAT | O_WRONLY | O_TRUNC, 0644);//0644?
		if (fd_output == -1)
			ft_error();
		close(fd[1]);
		if (dup2(fd_output, STDOUT_FILENO) == -1)
		{
			close(fd_output);
			perror("dup2");
			exit(1);
		}
		close(fd_output);
		if (dup2(fd[0], STDIN_FILENO) == -1)
		{
			close(fd[0]);
			perror("dup2");
			exit(1);
		}
		close(fd[0]);
		if (execve(path, command, envp) == -1)
			ft_error();
	}
	else
	{
		ft_error();
	}
	return (0);
}