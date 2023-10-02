document.addEventListener('DOMContentLoaded', function() {
    const socket = new WebSocket('ws://localhost:12345');

    socket.onopen = function() {
        console.log('Connected to the server');
    };

    socket.onerror = function(error) {
        console.log(`WebSocket Error: ${error}`);
    };

    function sendCommand(command) {
        if(socket.readyState === WebSocket.OPEN) {
            socket.send(command);
        }
    }

    document.getElementById('btnForward').addEventListener('click', function() {
        sendCommand('move_forward');
    });

    document.getElementById('btnBackward').addEventListener('click', function() {
        sendCommand('move_backward');
    });

    document.getElementById('btnLeft').addEventListener('click', function() {
        sendCommand('turn_left');
    });

    document.getElementById('btnRight').addEventListener('click', function() {
        sendCommand('turn_right');
    });

    document.getElementById('xSlider').addEventListener('input', function(event) {
        sendCommand(`x_axis_${event.target.value}`);
    });

    document.getElementById('ySlider').addEventListener('input', function(event) {
        sendCommand(`y_axis_${event.target.value}`);
    });
});
