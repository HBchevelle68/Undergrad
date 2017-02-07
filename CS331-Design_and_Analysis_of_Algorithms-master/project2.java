import java.util.Random;
import java.util.Scanner;
import java.util.*;

public class project2 {
	static int pivotposition = 0;
	static double toSec = 1000000000.0;
	static int n = 0;
	static int tester = 0;

	public static void main(String[] args) throws Exception {
		Random rand = new Random();
		Scanner kb = new Scanner(System.in);
		System.out.print("Enter size, must be multiple of 2 ");
		n = kb.nextInt();
		int[] s1 = chooseRand(n);
		int[] s2 = chooseRand(n);
		int[] s3 = chooseRand(n);
		int[] a4 = chooseRand(n);

		System.out.print("\n");
		double start = System.nanoTime();
		int k0 = Sel1(s1, s1.length, 0);
		int k1 = Sel1(s1, s1.length, n / 2);
		int k2 = Sel1(s1, s1.length, (3 * n / 4));
		int k3 = Sel1(s1, s1.length, n);
		double end = System.nanoTime();
		double total = ((end - start) / toSec);
		System.out.println("Select1 Algorithm: ");
		System.out.println("k0 = " + k0);
		System.out.println("k1 = " + k1);
		System.out.println("k2 = " + k2);
		System.out.println("k3 = " + k3);
		System.out.println("Time elapsed in seconds: " + total + "seconds");
		tester++;

		start = System.nanoTime();
		k0 = Sel2(s2, 0);
		k1 = Sel2(s2, (n / 2));
		k2 = Sel2(s2, (3 * n / 4));
		k3 = Sel2(s2, n);
		end = System.nanoTime();
		total = ((end - start) / toSec);
		System.out.println("Sel2 Algorithm: ");
		System.out.println("k0 = " + k0);
		System.out.println("k1 = " + k1);
		System.out.println("k2 = " + k2);
		System.out.println("k3 = " + k3);
		System.out.println("Time elapsed in seconds: " + total + "seconds");

		start = System.nanoTime();
		k0 = Sel3_1(s3, 0);
		k1 = Sel3_1(s3, (n / 2));
		k2 = Sel3_1(s3, (3 * n / 4));
		k3 = Sel3_1(s3, n);
		end = System.nanoTime();
		total = ((end - start) / toSec);
		System.out.println("Select3 Algorithm: ");
		System.out.println("k0 = " + k0);
		System.out.println("k1 = " + k1);
		System.out.println("k2 = " + k2);
		System.out.println("k3 = " + k3);
		System.out.println("Time elapsed in seconds: " + total + "seconds");

		start = System.nanoTime();
		k0 = Sel4(a4, 0);
		k1 = Sel4(a4, (n / 2));
		k2 = Sel4(a4, (3 * n / 4));
		k3 = Sel4(a4, n);
		end = System.nanoTime();
		total = ((end - start) / toSec);
		System.out.println("Select 4 Algorithm: ");
		System.out.println("k0 = " + k0);
		System.out.println("k1 = " + k1);
		System.out.println("k2 = " + k2);
		System.out.println("k3 = " + k3);
		System.out.println("Time elapsed in seconds: " + total + "seconds");

	}

	public static void Sel3_2(int[] a, int p, int q, int key) {
		int piv = partition(a, p, q);
		if (piv == key)
			return;
		if (p < piv - 1)
			Sel3_2(a, p, piv - 1, key);
		if (piv < q)
			Sel3_2(a, piv, q, key);
	}

	public static int partition(int[] a, int low, int high) {
		int piv = a[low];
		int p = low;
		int q = high;
		while (p <= q) {
			while (a[p] < piv)
				p++;
			while (a[q] > piv)
				q--;
			if (p <= q) {
				int temp = a[p];
				a[p] = a[q];
				a[q] = temp;
				p++;
				q--;
			}
		}
		return p;
	}

	public static int Sel1(int[] a, int n, int k) {
		mSort(a, 0, n - 1);
		if (k == 0) {
			return a[k];
		}
		return a[k - 1];
	}

	public static int Sel2(int[] a, int key) {
		int n = a.length - 1;
		if (key == 0) {
			itQSearch(a, 0, n, key);
			return a[key];
		}
		itQSearch(a, 0, n, key - 1);
		return a[key - 1];
	}

	public static int Sel3_1(int[] a, int k) {
		int n = a.length - 1;
		if (k == 0) {
			Sel3_2(a, 0, n, k);
			return a[k];
		}
		Sel3_2(a, 0, n, k - 1);
		return a[k - 1];

	}

	public static int Sel4(int[] a, int k) {
		int n = a.length - 1;
		if (k == 0) {
			mmQsearch(a, 0, n, k);
			return a[k];
		}
		mmQsearch(a, 0, n, k - 1);
		return a[k - 1];
	}

	public static void itQSearch(int[] a, int low, int high, int k) {
		LinkedList<Integer> t_linked = new LinkedList();
		t_linked.add(low);
		t_linked.add(high);
		while (t_linked.size() != 0) {
			low = t_linked.get(0);
			high = t_linked.get(1);
			t_linked.remove(0);
			t_linked.remove(0);
			int piv = partition(a, low, high);
			if (piv == k)
				return;
			if (low < piv - 1) {
				t_linked.add(0, low);
				t_linked.add(1, piv - 1);
			}
			if (piv < high) {
				t_linked.add(0, piv);
				t_linked.add(1, high);
			}
		}
	}

	public static int mmPiv(int[] a, int p, int q) {
		int[] t_array = a;
		int left = p;
		int right = q + 1;
		int num_M = (right - left) / 5;
		for (int i = 0; i < num_M; i++) {
			int tleft = left + i * 5;
			int tright = tleft + 5;
			if (tright > right)
				tright = right;
			int med = selIndex(t_array, tleft, tright, 2);
			int t_array_2 = t_array[i];
			t_array[i] = t_array[med];
			t_array[med] = t_array_2;
		}
		return selIndex(t_array, left, left + num_M, num_M / 2);
	}

	public static int selIndex(int[] a, int left, int right, int b) {
		int[] t_array = new int[right - left];
		int i, j;
		for (i = left, j = 0; i < right; i++, j++) {
			t_array[j] = a[i];
		}
		mSort(t_array, 0, t_array.length - 1);
		for (i = left; i < right; i++) {
			if (t_array[b] == a[i])
				break;
		}
		return i;
	}

	public static void mmQsearch(int[] a, int p, int q, int key) {
		int piv = med(a, p, q);
		if (piv == key)
			return;
		if (p < piv - 1)
			Sel3_2(a, p, piv - 1, key);
		if (piv < q)
			Sel3_2(a, piv, q, key);
	}

	public static int med(int[] a, int low, int high) {
		int pivot = mmPiv(a, low, high);
		int p = low;
		int q = high;
		while (p <= q) {
			while (a[p] < pivot)
				p++;
			while (a[q] > pivot) {
				q--;
			}
			if (p <= q) {
				int temp = a[p];
				a[p] = a[q];
				a[q] = temp;
				p++;
				--q;
			}
		}
		return p;
	}

	public static void mSort(int[] a, int low, int high) {
		int mid;
		if (low < high) {
			mid = (low + high) / 2;
			mSort(a, low, mid);
			mSort(a, mid + 1, high);
			merge(a, low, mid, high);
		}
	}

	public static int randGen(int a) {
		Random rand = new Random();
		return 1 + rand.nextInt(a);
	}

	public static void merge(int[] a, int low, int mid, int high) {
		int i = low;
		int j = mid + 1;
		int k = 0;
		int[] t_array = new int[1 + (high - low)];
		while (i <= mid && j <= high) {
			if (a[i] < a[j]) {
				t_array[k] = a[i];
				i++;
			} else {
				t_array[k] = a[j];
				j++;
			}
			k++;
		}
		while (i <= mid) {
			t_array[k] = a[(i++)];
			k++;
		}
		while (j <= high) {
			t_array[k++] = a[j++];
		}
		for (int p = 0; p < t_array.length; p++) {
			a[low + p] = t_array[p];
		}
	}

	public static int[] chooseRand(int num) {
		// creates a unique list of numbers(non-repeating)
		Set<Integer> opts = new HashSet<Integer>();
		int rand;
		while (opts.size() < num) {
			opts.add(randGen(num));
		}
		List<Integer> t_linked = new ArrayList(opts);
		Collections.shuffle(t_linked);
		int[] ret = new int[opts.size()];
		for (int i = 0; i < ret.length; i++) {
			ret[i] = t_linked.get(i);
		}
		return ret;
	}
}
