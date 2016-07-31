priority = 10;
group = 'group';

test_var = 0;

function start()
    delay(1, on_delay);    
end

function on_delay()
    test_var = 1;
    finish();
end