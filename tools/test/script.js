const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

const rangeKp = document.getElementById('kp');
const rangeKi = document.getElementById('ki');
const rangeKd = document.getElementById('kd');
const checkboxFeedback = document.getElementById('feedback');
const checkboxFeedforward = document.getElementById('feedforward');

const car = {
    x: 100,
    y: 300,
    speed: 0,
    maxSpeed: 0.5,
    acceleration: 0.1,
    deceleration: 0.1
};

const obstacle = {
    x: 600,
    y: 300,
    radius: 50
};

let integral = 0;
let lastError = 0;

function reset() {
    integral = 0;
    lastError = 0;
}

function setup() {
    canvas.width = 800;
    canvas.height = 600;
    ctx.fillStyle = 'black';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    rangeKp.value = 0;
    rangeKi.value = 0;
    rangeKd.value = 0;
    checkboxFeedback.checked = false;
    checkboxFeedforward.checked = false;

    rangeKp.addEventListener('input', reset.bind(this));
    rangeKi.addEventListener('input', reset.bind(this));
    rangeKd.addEventListener('input', reset.bind(this));
    checkboxFeedback.addEventListener('change', reset.bind(this));
    checkboxFeedforward.addEventListener('change', reset.bind(this));

    loop();
}

function pid(input, setpoint) {
    let error = setpoint - input;
    integral += error;
    let derivative = error - lastError;
    lastError = error;

    let kp = parseFloat(rangeKp.value);
    let ki = parseFloat(rangeKi.value);
    let kd = parseFloat(rangeKd.value);

    // Enable feedforward control
    if (checkboxFeedforward.checked) {
        return kp * error + ki * integral + kd * derivative + 0.1;
    }

    // Enable feedback control
    if (checkboxFeedback.checked) {
        return kp * error + ki * integral + kd * derivative - 0.1;
    }

    return kp * error + ki * integral + kd * derivative;
}

function loop() {
    ctx.fillStyle = 'black';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Go to the right, but avoid the obstacle
    let distance = Math.sqrt(Math.pow(obstacle.x - car.x, 2) + Math.pow(obstacle.y - car.y, 2));
    let setpoint = obstacle.radius * 3;
    let control = pid(distance, setpoint);

    car.speed = Math.min(car.speed + control, car.maxSpeed);
    car.x -= car.speed;

    if (car.x < 0) {
        car.x = 0;
    } else if (car.x > obstacle.x - obstacle.radius - 10) {
        car.x = obstacle.x - obstacle.radius - 10;
    }

    ctx.fillStyle = 'red';
    ctx.beginPath();
    ctx.arc(obstacle.x, obstacle.y, obstacle.radius, 0, Math.PI * 2);
    ctx.fill();

    ctx.fillStyle = 'blue';
    ctx.beginPath();
    ctx.arc(car.x, car.y, 10, 0, Math.PI * 2);
    ctx.fill();

    requestAnimationFrame(loop);
}

setup()