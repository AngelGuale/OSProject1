/* 
 * File:   generic.h
 * Author: Angel
 *
 * Created on June 6, 2012, 10:19 AM
 */

#ifndef GENERIC_H
#define	GENERIC_H

#ifdef	__cplusplus
extern "C" {
#endif
    typedef void * Generic;
    
    Generic integerNew(int a);
    int integerGet(Generic g);
    void integerSet(Generic g, int n);

    typedef int (*cmpfn) (Generic, Generic);
    typedef void (*printfn) (Generic);

#ifdef	__cplusplus
}
#endif

#endif	/* GENERIC_H */

