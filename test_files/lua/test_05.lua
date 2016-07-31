priority = 10;
group = 'group';

is_canceled = 0;

on_func1_count = 0;
on_func2_count = 0;
on_func3_count = 0;
on_func4_count = 0;

on_error1_count = 0;
on_error2_count = 0;
on_error3_count = 0;
on_error4_count = 0;

relay_num1 = -1;
relay_num2 = -1;
relay_state = -1;

function start()
    if_happens(1, on_relay_changed(nil, nil), ok_func, error_func1);
    if_happens(1, on_relay_changed(nil, nil), ok_func2, error_func2);
    if_happens(1, on_relay_changed(10, 20), ok_func3, error_func3);
    if_happens(0.5, on_relay_changed(nil, nil), ok_func4, error_func4);
end

function ok_func(relay_num, _relay_state)
	on_func1_count = on_func1_count + 1;
  relay_num1 = relay_num;
  relay_state = _relay_state;
end

function ok_func2(relay_num)
  on_func2_count = on_func2_count + 1;
  relay_num2 = relay_num;
end

function ok_func3()
	on_func3_count = on_func3_count + 1;
end

function ok_func4()
	on_func4_count = on_func4_count + 1;
end


function error_func1()
  on_error1_count = on_error1_count + 1;
end

function error_func2()
  on_error2_count = on_error2_count + 1;
end

function error_func3()
  on_error3_count = on_error3_count + 1;
end

function error_func4()
  on_error4_count = on_error4_count + 1;
end

function cleanup_on_cancel()
  is_canceled = 1;
end