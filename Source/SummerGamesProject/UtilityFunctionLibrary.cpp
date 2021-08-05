// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityFunctionLibrary.h"

int UUtilityFunctionLibrary::modulo(int a, int b)
{
    int m = a % b;
    return (a < 0) ? b + m : m;
}