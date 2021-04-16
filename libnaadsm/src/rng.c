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


//JFD #include <naadsm/rng.h>
//JFD #include <sprng.h>

//JFD TRngVoid_1_Int rng_read_seed;

/*JFD
 * Replace the naadsm random number generator with the C++ 11 standard
 */
#include <QRandomGenerator>

/**
 * Wraps sprng() in a function that can be stored in a gsl_rng_type object.
 */
//JFD double
//JFD sprng_as_get_double (void *state)
//JFD {
//JFD   RAN_gen_t *rng;
//JFD
//JFD   rng = (RAN_gen_t *) state;
//JFD   return RAN_num (rng);
//JFD }



/**
 * Wraps isprng() in a function that can be stored in a gsl_rng_type object.
 */
//JFD unsigned long int
//JFD sprng_as_get (void *dummy)
//JFD {
//JFD   if( dummy ) {} // Avoid a compiler warning.
//JFD   return (unsigned long int)( isprng () );
//JFD }



/**
 * Creates a new random number generator object.
 *
 * @param seed a seed value.  Use -1 to indicate that a seed should be picked
 *  automatically.
 * @return a random number generator.
 */
//JFD RAN_gen_t *
//JFD RAN_new_generator (int seed)
//JFD {
//JFD   RAN_gen_t *self;
//JFD
//JFD   if (seed == -1)
//JFD     seed = make_sprng_seed ();
//JFD
//JFD   if (NULL != rng_read_seed)
//JFD     rng_read_seed (seed);
//JFD
//JFD   g_assert (init_sprng (SPRNG_LFG, seed, SPRNG_DEFAULT) != NULL);
//JFD
//JFD   self = g_new (RAN_gen_t, 1);
//JFD   self->fixed = FALSE;
//JFD
//JFD   /* Fill in the GSL-compatibility fields. */
//JFD   self->as_gsl_rng_type.name = "SPRNG2.0";
//JFD   self->as_gsl_rng_type.max = 2147483647;
//JFD   self->as_gsl_rng_type.min = 0;
//JFD   self->as_gsl_rng_type.size = 0;
//JFD   self->as_gsl_rng_type.set = NULL;
//JFD   self->as_gsl_rng_type.get = sprng_as_get;
//JFD   self->as_gsl_rng_type.get_double = sprng_as_get_double;
//JFD
//JFD   self->as_gsl_rng.type = &(self->as_gsl_rng_type);
//JFD   self->as_gsl_rng.state = self;
//JFD
//JFD   return self;
//JFD }



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


//JFD unsigned long int
//JFD RAN_int (RAN_gen_t * rng, const unsigned long int min, const unsigned long int max )
//JFD {
//JFD   unsigned long int range = max - min + 1;
//JFD   return gsl_rng_uniform_int (RAN_generator_as_gsl (rng), range) + min;
//JFD }



/**
 * Returns a pointer that allows the generator to be used as a GNU Scientific
 * Library generator.
 *
 * @param gen a random number generator.
 * @return a GSL random number generator.
 */
//JFD gsl_rng *
//JFD RAN_generator_as_gsl (RAN_gen_t * gen)
//JFD {
//JFD   return &(gen->as_gsl_rng);
//JFD }



/**
 * Causes a random number generator to always return a particular value.
 *
 * @param gen a random number generator.
 * @param value the value to fix.
 */
//JFD void
//JFD RAN_fix (RAN_gen_t * gen, double value)
//JFD {
//JFD   gen->fixed = TRUE;
//JFD   gen->fixed_value = value;
//JFD }



/**
 * Causes a random number generator to return random values, reversing the
 * effect of RAN_fix().
 *
 * @param gen a random number generator.
 */
//JFD void
//JFD RAN_unfix (RAN_gen_t * gen)
//JFD {
//JFD   gen->fixed = FALSE;
//JFD }



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


//JFD DLL_API void
//JFD set_rng_read_seed (TRngVoid_1_Int fn)
//JFD {
//JFD   rng_read_seed = fn;
//JFD }


//JFD void
//JFD clear_rng_fns (void)
//JFD {
//JFD   set_rng_read_seed (NULL);
//JFD }


/* end of file rng.c */

#ifdef __cplusplus
}
#endif


