/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   intersect_cyl.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olozano- <olozano-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/29 15:19:40 by olozano-          #+#    #+#             */
/*   Updated: 2021/12/09 15:09:09 by olozano-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minirt.h"

int	cy_calculs(t_v3 origin, t_v3 ray, t_objs *obj, double x[2])
{
	t_v3	v;
	t_v3	u;
	double	a;
	double	b;
	double	c;

	v = scale_v(obj->orient, dot_p(ray, obj->orient));
	v = sub(ray, v);
	a = dot_p(sub(origin, obj->coord), obj->orient);
	u = scale_v(obj->orient, a);
	u = sub(sub(origin, obj->coord), u);
	a = dot_p(v, v);
	b = 2 * dot_p(v, u);
	c = dot_p(u, u) - pow(obj->params.x / 2, 2);
	x[0] = (sqrt(pow(b, 2) - 4 * a * c) - b) / (2 * a);
	x[1] = (-sqrt(pow(b, 2) - 4 * a * c) - b) / (2 * a);
	return (1);
}

double	best_body(double dist[2], double x[2], t_objs *obj)
{
	if (!((dist[0] >= 0 && dist[0] <= obj->params.y && x[0] > 0.001)
			|| (dist[1] >= 0 && dist[1] <= obj->params.y && x[0] > 0.001)))
		return (-1);
	if (dist[0] >= 0 && dist[0] <= obj->params.y && x[0] > 0.001
		&& dist[1] >= 0 && dist[1] <= obj->params.y && x[1] > 0.001)
	{
		if (x[0] < x[1])
			return (x[0]);
		else
			return (x[1]);
	}
	else if (dist[0] >= 0 && dist[0] <= obj->params.y && x[0] > 0.001)
		return (x[0]);
	else
		return (x[1]);
}

double	check_caps(t_v3 origin, t_v3 ray, t_objs *obj, t_v3 ccc)
{
	double	dd;
	t_v3	aux;

	if (ccc.x == obj->coord.x && ccc.y == obj->coord.y)
		dd = compute_plane(origin, ray, obj, 0);
	else
		dd = compute_plane(origin, ray, obj, 1);
	if (dd < 0.001)
		return (-1);
	aux = add_v(origin, scale_v(ray, dd));
	if (dd > 0.001 && distance3(aux, ccc) <= obj->params.x / 2)
		return (dd);
	else
		return (-1);
}

//normalize(vsub(vsub(scal_x_vec(x, d),
//			scal_x_vec(dist, lst->fig.cy.nv)), vsub(lst->fig.cy.c, o)))

/*
 t = dot((hit_pt - cy.bottom_center), cy.ori); // cy.ori should be normalized and so has the length of 1.
    pt = cy.bottom_center + t * cy.ori;
    surface_normal = normalize(hit_pt - pt))); */

double	best_with_caps(t_v3 origin, t_v3 ray, t_objs *obj, double inter_body)
{
	double	dd[2];
	double	caps;
	t_v3	ccc;
	t_v3	aux;

	dd[0] = check_caps(origin, ray, obj, obj->coord);
	ccc = add_v(obj->coord, (scale_v(obj->orient, obj->params.y)));
	dd[1] = check_caps(origin, ray, obj, ccc);
	caps = -1;
	aux = add_v(origin, scale_v(ray, dd[1]));
	
	if (dd[0] > 0.001 && (dd[0] < dd[1] || dd[1] < 0.001))
		caps = dd[0];
	else if (dd[1] > 0.001 && (dd[1] < dd[0] || dd[0] < 0.001))
		caps = dd[1];
	obj->normal = v_dup(obj->orient);
	if (caps == dd[1])
		obj->normal = scale_v(obj->normal, -1);
	if (caps > 0.001 && (caps < inter_body || inter_body < 0.001))
		return (caps);
	caps = dot_p(sub(add_v(origin, scale_v(ray, inter_body)), obj->coord), obj->orient);
	aux = add_v(obj->coord, scale_v(obj->orient, caps));
	obj->normal = normalize(sub(add_v(origin, scale_v(ray, inter_body)), aux));


	//obj->normal = normalize(sub(add_v(origin, scale_v(ray, inter_body)), add_v(obj->coord,
	//	scale_v (obj->coord, dot_p(sub(add_v(origin, scale_v(ray, inter_body)), obj->coord), obj->orient)))));
	return (inter_body);
}

double	inter_cylinder(t_v3 origin, t_v3 ray, t_objs *obj)
{
	double	dist[2];
	t_v3	aux;
	t_v3	aux2;
	double	x[2];
	t_v3	final_dist;

	cy_calculs(origin, ray, obj, x);
	aux = sub(scale_v(ray, x[0]), sub(obj->coord, origin));
	aux2 = sub(scale_v(ray, x[1]), sub(obj->coord, origin));
	dist[0] = dot_p(obj->orient, aux);
	dist[1] = dot_p(obj->orient, aux2);
	return (best_with_caps(origin, ray, obj, best_body(dist, x, obj)));
}
