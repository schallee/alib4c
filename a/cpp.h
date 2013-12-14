/*
 *  Copyright (C) 2012 Ed Schaller <schallee@darkmist.net>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef a_cpp_h
#define a_cpp_h	1

/* useful macro constructs */
#define a_concat_m(a,b)	a ## b
#define a_val_concat_m(a,b)	a_concat_m(a,b)
#define a_line_var_m(var)	a_val_concat_m(a_val_concat_m(var, _), __LINE__)
#define a_line_var(var)		a_line_var_m(var)
#define a_var_val2str_helper(var)	#var
#define a_var_val2str(var)	a_var_val2str_helper(var)
#define a_line_as_str()		a_var_val2str(__LINE__)

/* expontents */
#define a_exp_0_m(base)	1
#define a_exp_1_m(base)		base
#define a_exp_2_m(base)		base * a_exp_1_m(base)
#define a_exp_3_m(base)		base * a_exp_2_m(base)
#define a_exp_4_m(base)		base * a_exp_3_m(base)
#define a_exp_5_m(base)		base * a_exp_4_m(base)
#define a_exp_6_m(base)		base * a_exp_5_m(base)
#define a_exp_7_m(base)		base * a_exp_6_m(base)
#define a_exp_8_m(base)		base * a_exp_7_m(base)
#define a_exp_9_m(base)		base * a_exp_8_m(base)
#define a_exp_10_m(base)	base * a_exp_9_m(base)
#define a_exp_11_m(base)	base * a_exp_10_m(base)
#define a_exp_12_m(base)	base * a_exp_11_m(base)
#define a_exp_13_m(base)	base * a_exp_12_m(base)
#define a_exp_14_m(base)	base * a_exp_13_m(base)
#define a_exp_15_m(base)	base * a_exp_14_m(base)
#define a_exp_16_m(base)	base * a_exp_15_m(base)
#define a_exp_17_m(base)	base * a_exp_16_m(base)
#define a_exp_18_m(base)	base * a_exp_17_m(base)
#define a_exp_19_m(base)	base * a_exp_18_m(base)
#define a_exp_20_m(base)	base * a_exp_19_m(base)
#define a_exp_21_m(base)	base * a_exp_20_m(base)
#define a_exp_22_m(base)	base * a_exp_21_m(base)
#define a_exp_23_m(base)	base * a_exp_22_m(base)
#define a_exp_24_m(base)	base * a_exp_23_m(base)
#define a_exp_25_m(base)	base * a_exp_24_m(base)
#define a_exp_26_m(base)	base * a_exp_25_m(base)
#define a_exp_27_m(base)	base * a_exp_26_m(base)
#define a_exp_28_m(base)	base * a_exp_27_m(base)
#define a_exp_29_m(base)	base * a_exp_28_m(base)
#define a_exp_30_m(base)	base * a_exp_29_m(base)
#define a_exp_31_m(base)	base * a_exp_30_m(base)
#define a_exp_32_m(base)	base * a_exp_31_m(base)
#define	a_exp_m(base,exp)	(a_exp_ ## exp ## _m((base)))

/* flags */
#define a_flag_m(exp)			(0x1 << exp)
#define a_bool2flag_m(val,exp)		(val ? a_flag_m(exp) : 0)
#define a_flag2bool_m(val,exp)		(a_flag_m(exp) & val)
#define a_set_flag_m(var,flag)		((var) |= (flag))
#define a_clear_flag_m(var,flag)	((var) &= ~(flag))
#define a_flag_set_m(var,flag)		((var) & (flag))

/* offsets */
#define a_byte_offset_m(type,ptr,off)		((type *)(((uint8_t *)(ptr)) + off))
#define a_get_offset_m(type,ptr,off)		(*(a_byte_offset_m(type,ptr,off)))
#define a_set_offset(type,ptr,off,val)		a_get_offset_m(type,ptr,off) = val

/* avoiding c++ name mangling */
#ifdef __cplusplus
#	define a_begin_c_decls_m		extern "C"	{
#	define a_end_c_decls_m			}
#else
#	define a_begin_c_decls_m
#	define a_end_c_decls_m
#endif
#define a_begin_c_declarations_m	a_begin_c_decls_m
#define a_start_c_decls_m		a_begin_c_decls_m
#define a_end_c_declarations_m		a_end_c_decls_m

/* min/max */
#define a_min_m(a,b)		((a)<(b) ? (a) : (b))
#define a_max_m(a,b)		((a)>(b) ? (a) : (b))

/* wrappers */
#define a_call_return_m(run,val)	do	\
					{	\
						run;	\
						return (val);	\
					} while(0)

#endif
