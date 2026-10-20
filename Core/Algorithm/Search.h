#pragma once
#include <functional>
#include <vector>

namespace Novot {
	template <typename T>
	int BinSearch(const std::vector<T>& arr, const T& target, const std::function<bool(const T& v, const T& target)>& comp = std::less<T>()) {
		int ng = -1;
		int ok = static_cast<int>(arr.size());
		for (int i = 0; i < arr.size(); ++i) {
			int mid = (ok + ng) / 2;
			if (comp(arr[mid], target)) {
				ng = mid;
			}
			else {
				ok = mid;
			}
		}
		return ok;
	}
}