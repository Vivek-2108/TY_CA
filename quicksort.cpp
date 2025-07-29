#include <iostream>
#include <cstdlib>  
using namespace std;

int callCount = 0; 

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(int arr[], int low, int high) {
    callCount++;
    if (low < high) {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main() {
    const int n = 1000; 
    int arr[n];

    
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000;
    }

    quickSort(arr, 0, n - 1);

    cout << "Sorted first 10 elements: ";
    printArray(arr, 10); 

    cout << "Total recursive calls: " << callCount << endl;
    return 0;
}
