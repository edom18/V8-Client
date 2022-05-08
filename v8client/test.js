function update()
{
    return "update";
}

function test() {
    api.position.x = 15;
    return "test : " + api.position.x;
    //var a = 5;
    //var b = 15;
    //var c = 23.3;

    //api.log("This is from JavaScript!!!\n");

    //api.log("-----------\n")

    //var e = new JSBindTest();
    //api.log(e.age + "\n");

    //e.age = 42;
    //api.log(e.age + ", updated.\n");

    //api.log("Object: " + e + "\n");

    //api.log(api);

    //return "Hello!" + (a + b + c);
}

test();