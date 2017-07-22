/** @file rng.c
 * Functions for getting random numbers.
 *
 * @author Neil Harvey <neilharvey@gmail.com><br>
 *   Grid Computing Research Group<br>
 *   Department of Computing & Information Science, University of Guelph<br>
 *   Guelph, ON N1G 2W1<br>
 *   CANADA
 * @version 0.1
 * @date October 2005
 *
 * Copyright &copy; University of Guelph, 2005-2008
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <naadsm/rng.h>
#include <sprng.h>

TRngVoid_1_Int rng_read_seed;

/**
 * Wraps sprng() in a function that can be stored in a gsl_rng_type object.
 */
double
sprng_as_get_double (void *state)
{
  RAN_gen_t *rng;

  rng = (RAN_gen_t *) state;
  return RAN_num (rng);
}



/**
 * Wraps isprng() in a function that can be stored in a gsl_rng_type object.
 */
unsigned long int
sprng_as_get (void *dummy)
{
  if( dummy ) {} // Avoid a compiler warning.
  return isprng ();
}



/**
 * Creates a new random number generator object.
 *
 * @param seed a seed value.  Use -1 to indicate that a seed should be picked
 *  automatically.
 * @return a random number generator.
 */
RAN_gen_t *
RAN_new_generator (int seed)
{
  RAN_gen_t *self;

  if (seed == -1)
    seed = make_sprng_seed ();
    
  if (NULL != rng_read_seed)
    rng_read_seed (seed);  
  
  g_assert (init_sprng (SPRNG_LFG, seed, SPRNG_DEFAULT) != NULL);

  self = g_new (RAN_gen_t, 1);
  self->fixed = FALSE;

  /* Fill in the GSL-compatibility fields. */
  self->as_gsl_rng_type.name = "SPRNG2.0";
  self->as_gsl_rng_type.max = 2147483647;
  self->as_gsl_rng_type.min = 0;
  self->as_gsl_rng_type.size = 0;
  self->as_gsl_rng_type.set = NULL;
  self->as_gsl_rng_type.get = sprng_as_get;
  self->as_gsl_rng_type.get_double = sprng_as_get_double;

  self->as_gsl_rng.type = &(self->as_gsl_rng_type);
  self->as_gsl_rng.state = self;

  return self;
}



/**
 * Returns a random number in [0,1).
 *
 * @param gen a random number generator.
 * @return a random number in [0,1).
 */
double
RAN_num (RAN_gen_t * gen)
{
  if (gen->fixed)
    return gen->fixed_value;
  else
    return sprng ();
}


unsigned long int
RAN_int (RAN_gen_t * rng, const unsigned long int min, const unsigned long int max )
{
  unsigned long int range = max - min + 1;
  return gsl_rng_uniform_int (RAN_generator_as_gsl (rng), range) + min;
}



/**
 * Returns a pointer that allows the generator to be used as a GNU Scientific
 * Library generator.
 *
 * @param gen a random number generator.
 * @return a GSL random number generator.
 */
gsl_rng *
RAN_generator_as_gsl (RAN_gen_t * gen)
{
  return &(gen->as_gsl_rng);
}



/**
 * Causes a random number generator to always return a particular value.
 *
 * @param gen a random number generator.
 * @param value the value to fix.
 */
void
RAN_fix (RAN_gen_t * gen, double value)
{
  gen->fixed = TRUE;
  gen->fixed_value = value;
}



/**
 * Causes a random number generator to return random values, reversing the
 * effect of RAN_fix().
 *
 * @param gen a random number generator.
 */
void
RAN_unfix (RAN_gen_t * gen)
{
  gen->fixed = FALSE;
}



/**
 * Deletes a random number generator from memory.
 *
 * @param gen a random number generator.
 */
void
RAN_free_generator (RAN_gen_t * gen)
{
  if (gen != NULL)
    g_free (gen);
}


DLL_API void
set_rng_read_seed (TRngVoid_1_Int fn)
{
  rng_read_seed = fn;
}


void
clear_rng_fns (void)
{
  set_rng_read_seed (NULL);
}


/* end of file rng.c */

#ifdef __cplusplus
}
#endif


