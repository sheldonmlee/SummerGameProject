// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityFunctionLibrary.h"

int UUtilityFunctionLibrary::modulo(int a, int b)
{
    if (b == 1) return 0;
    int m = a % b;
    return (a < 0) ? b + m : m;
}

TArray<FText> UUtilityFunctionLibrary::intToTextArray(TArray<int32> arr)
{
    TArray<FText> return_arr;
    for (int i = 0; i != arr.Num(); i++) 
        return_arr.Add(FText::FromString(FString::FromInt(arr[i])));
    return return_arr;
}