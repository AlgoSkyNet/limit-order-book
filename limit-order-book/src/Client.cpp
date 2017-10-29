#include "stdafx.h"

#include "Client.h"

extern double decimal_round(double x, int points);

ClientOrder Client::Query(const std::vector<double>& bid_order_sizes,
	const std::vector<double>& ask_order_sizes)
{
	double upper_intensity{ _upper_intensity_base };

	for (int i = 0; i != _limit; ++i)
	{
		_event[2 + 2 * _limit + i] = _cancel_intensity[i] * bid_order_sizes[i];
		_event[2 + 3 * _limit + i] = _cancel_intensity[i] * ask_order_sizes[i];

		upper_intensity += _cancel_intensity[i] * (bid_order_sizes[i] + ask_order_sizes[i]);
	}

	double unif = random_check() / RAND_MAX;
	double new_event = -log(unif) / upper_intensity;

	double new_event_type = random_check() / RAND_MAX;

	double pick_event{ 0.0 };
	int pick{ 0 };

	while (decimal_round(pick_event, 5) <= decimal_round(new_event_type, 5))
	{
		pick_event += _event[pick] / upper_intensity;
		++pick;
	}

	--pick;
	_client_order.time += new_event;
	_client_order.size = size_distribution();

	if (pick < 2)
	{
		// 0 - Bid or 1 - Ask mapped to 1 or 4 of type_identifier of ClientOrder
		_client_order.type_identifier = pick * 3 + 1; 
		_client_order.price = 0.0;
		_client_order.id = _id[_client_order.type_identifier];
	}
	else if (pick < 2 + _limit)
	{
		_client_order.type_identifier = 0;
		_client_order.price = pick - 2;
		_client_order.id = _id[_client_order.type_identifier]++;
	}
	else if (pick < 2 + 2 * _limit)
	{
		_client_order.type_identifier = 3;
		_client_order.price = pick - 2 - _limit;
		_client_order.id = _id[_client_order.type_identifier]++;
	}
	else if (pick < 2 + 3 * _limit)
	{
		_client_order.type_identifier = 2;
		_client_order.price = pick - 2 - 2 * _limit;
		_client_order.id = _id[_client_order.type_identifier]++;
	}
	else
	{
		_client_order.type_identifier = 5;
		_client_order.price = pick - 2 - 3 * _limit;
		_client_order.id = _id[_client_order.type_identifier]++;
	}

	_client_order.price *= _default_spread;

	return _client_order;
}

inline double Client::random_check()
{
	double random = rand();
	if (random == 0 || random == RAND_MAX)
	{
		return random_check();
	}
	else
	{
		return random;
	}
}

inline double Client::size_distribution()
{
	return 1.0;
}


